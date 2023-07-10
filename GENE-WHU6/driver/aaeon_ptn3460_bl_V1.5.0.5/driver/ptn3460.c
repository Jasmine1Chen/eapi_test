/*
 * Copyright (c) 2016, AAEON CORPORATION.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <linux/fb.h>
#include <linux/backlight.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/err.h>
#include <linux/regulator/consumer.h>
#include <linux/regmap.h>
#include <linux/delay.h>

#define PTN3460_BL_MAX_CURR	0xFFF
#define PTN3460_MAX_BRIGHTNESS 0xFF

#define BL_ON   0
#define BL_OFF  1

// Addresses to scan
static const unsigned short normal_i2c[] = {
	0x20, 0x60, I2C_CLIENT_END
};

// NXP PTN3460 has no vendor id/device id information
// in the registers. Hence, using the embedded EDID
// information to instead of the identities.
static const u8 PTN3460_EDID_INFO[] = {
	0x00, // EDID HEAD[0]
	0xff, // EDID HEAD[1]
	0xff, // EDID HEAD[2]
	0xff, // EDID HEAD[3]
	0xff, // EDID HEAD[4]
	0xff, // EDID HEAD[5]
	0xff, // EDID HEAD[6]
	0x00, // EDID HEAD[7]
};

// Return 0 if detection is successful, -ENODEV otherwise
static int ptn3460_bl_detect(struct i2c_client *client,
			 struct i2c_board_info *info)
{
	int data, i;
	struct i2c_adapter *adapter = client->adapter;
	unsigned short address = client->addr;

	if (!i2c_check_functionality(adapter, I2C_FUNC_SMBUS_BYTE_DATA))
		return -ENODEV;

	for (i = 0; i < sizeof(PTN3460_EDID_INFO); i++) {
		data = i2c_smbus_read_byte_data(client, (u8)i);
		if (PTN3460_EDID_INFO[i] != data) {
			dev_info(&adapter->dev,
					"ptn3460: Detection failed at addr 0x%02hx, check %s\n",
					address, "ptn3460 edid");
			return -ENODEV;
		}
	}

	strlcpy(info->type, "ptn3460", I2C_NAME_SIZE);
	dev_info(&adapter->dev, "Found ptn3460 at 0x%02hx\n", address);

	return 0;
}

struct ptn3460_bridge {
	struct i2c_client *client;
	struct regulator *v12;
	struct backlight_device *bl;
};

static const struct i2c_device_id ptn3460_id[] = {
	{ "ptn3460", 0 },
	{},
};
MODULE_DEVICE_TABLE(i2c, ptn3460_id);

static int ptn3460_backlight_get_brightness(struct backlight_device *bl)
{
	struct ptn3460_bridge *ptn3460 = dev_get_drvdata(&bl->dev);
	int ret, brightness ,bl_converter;
	
	ret = i2c_smbus_read_byte_data(ptn3460->client,0x90);
	if (ret < 0) {
		dev_err(&ptn3460->client->dev, "failed writing register: %d\n", ret);
		return ret;
	}
	brightness = ((~ret) & 0x0F) << 8;
	ret = i2c_smbus_read_byte_data(ptn3460->client,0x91);
	if (ret < 0) {
		dev_err(&ptn3460->client->dev, "failed writing register: %d\n", ret);
		return ret;
	}
	brightness = brightness | ((~ret) & 0x00FF );
	bl_converter = brightness / (PTN3460_BL_MAX_CURR / PTN3460_MAX_BRIGHTNESS);
	
	return bl_converter;
}

static int ptn3460_backlight_update_status(struct backlight_device *bl)
{
	struct ptn3460_bridge *ptn3460 = dev_get_drvdata(&bl->dev);
	int ret = 0, bl_converter, backlight_status, brightness = bl->props.brightness;
	
	if (bl->props.power != FB_BLANK_UNBLANK ||
		bl->props.state & (BL_CORE_SUSPENDED | BL_CORE_FBBLANK))
	{
		brightness = 0;
		backlight_status = BL_OFF;
		ret = i2c_smbus_write_byte_data(ptn3460->client,0x95, backlight_status);	// Backlight Off

		if (ret < 0) {
			dev_err(&ptn3460->client->dev, "failed control backlight off: %d\n", ret);
			return ret;
		}
	}
	else
	{
		backlight_status = BL_ON;
		
		ret = i2c_smbus_write_byte_data(ptn3460->client,0x95, backlight_status);	// Backlight On

		if (ret < 0) {
			dev_err(&ptn3460->client->dev, "failed control backlight on: %d\n", ret);
			return ret;
		}
	}
	
		bl_converter = brightness * PTN3460_BL_MAX_CURR / PTN3460_MAX_BRIGHTNESS;
		brightness = ((~bl_converter) & 0x0FFF) >> 8;
		ret = i2c_smbus_write_byte_data(ptn3460->client,0x90, brightness );		// Brightness Control
		if (ret < 0) {
				dev_err(&ptn3460->client->dev, "failed writing register: %d\n", ret);
				return ret;
		}
	
		brightness = ((~bl_converter) & 0x00FF );
		ret = i2c_smbus_write_byte_data(ptn3460->client,0x91, brightness);		// Brightness Control
		if (ret < 0) {
				dev_err(&ptn3460->client->dev, "failed writing register: %d\n", ret);
				return ret;
		}
	
	return ret;
}

static const struct backlight_ops ptn3460_backlight_ops = {
	.get_brightness	= ptn3460_backlight_get_brightness,
	.update_status	= ptn3460_backlight_update_status,
};

static int ptn3460_bl_probe(struct i2c_client *client,
	const struct i2c_device_id *id)
{
	struct device *dev = &client->dev;
	struct ptn3460_bridge *ptn3460;
	char bl_dev_name[I2C_NAME_SIZE];
	
	int ret;
	memset(bl_dev_name, 0, I2C_NAME_SIZE);
	snprintf(bl_dev_name, I2C_NAME_SIZE, "%s_bl-%s", dev_driver_string(dev), dev_name(dev));

	ptn3460 = devm_kzalloc(dev, sizeof(*ptn3460), GFP_KERNEL);
	if (!ptn3460)
			return -ENOMEM;
		
	ptn3460->client = client;

	ptn3460->v12 = devm_regulator_get(dev, "vdd33");
	if (IS_ERR(ptn3460->v12)) {
		dev_info(dev, "no 3.3v regulator found for PTN3460\n");
		ptn3460->v12 = NULL;
	}

	ptn3460->bl = backlight_device_register(bl_dev_name,
			dev, ptn3460, &ptn3460_backlight_ops,NULL);
	
	if (IS_ERR(ptn3460->bl)) {
		dev_err(dev, "failed to register backlight\n");
		ret = PTR_ERR(ptn3460->bl);
		ptn3460->bl = NULL;
		return ret;
	}
	
	ptn3460->bl->props.max_brightness = PTN3460_MAX_BRIGHTNESS;
	ptn3460->bl->props.brightness = 0x80;


	i2c_set_clientdata(client, ptn3460);
	
	return 0;
}

static int ptn3460_bl_remove(struct i2c_client *client)
{
	struct ptn3460_bridge *ptn3460 = i2c_get_clientdata(client);

	if (ptn3460->bl)
		backlight_device_unregister(ptn3460->bl);

	return 0;
}

static struct i2c_driver ptn3460_bl_driver = {
	.driver = {
		.name	= "ptn3460",
		.owner	= THIS_MODULE,
	},
	.id_table = ptn3460_id,
	.probe	= ptn3460_bl_probe,
	.remove	= ptn3460_bl_remove,
	.class = I2C_CLASS_SPD, // I2C_CLASS_SPD for SMBus (i801)
	.detect = ptn3460_bl_detect,
	.address_list = normal_i2c,
};
module_i2c_driver(ptn3460_bl_driver);


MODULE_AUTHOR("Menghui Wu <Menghui_Wu@asus.com>");
MODULE_DESCRIPTION("PTN3460 Backlight display driver for SMBus");
MODULE_LICENSE("GPL");
