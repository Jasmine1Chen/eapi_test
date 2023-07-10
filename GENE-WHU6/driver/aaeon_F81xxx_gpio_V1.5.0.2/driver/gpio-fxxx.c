/***************************************************************************
* GPIO driver for Fintek Super-I/O F75113, F81801, F81866 and F81966       *	
*                                                                          *
* Copyright (C) 2023 Charlie Chan <charlie_chan@acrosser.com.tw> 				       *
*
*	   Based on Nick Chen <nick1chen@aaeon.com.tw>																		   *
*          Based on gpio-f81866.c by Jason Wang        					   *
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
* This program is distributed in the hope that it will be useful,          *
* but WITHOUT ANY WARRANTY; without even the implied warranty of           *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
* GNU General Public License for more details.                             *
*                                                                          *
* You should have received a copy of the GNU General Public License        *
* along with this program; if not, write to the                            *
* Free Software Foundation, Inc.,                                          *
* 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.                 *
***************************************************************************/

#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/gpio.h>
#include <linux/version.h>

#define DRVNAME "gpio-fxxx"

/*
 * Super-I/O registers
 */
#define SIO_LDSEL		0x07	/* Logical device select */
#define SIO_DEVID		0x20	/* Device ID (2 bytes) */
#define SIO_DEVREV		0x22	/* Device revision */
#define SIO_MANID		0x23	/* Fintek ID (2 bytes) */

#define SIO_LOCK_KEY			0xAA	/* Key to disable Super-I/O */
#define SIO_LD_GPIO_F75113  	0x01    /* GPIO logical device */
#define	SIO_UNLOCK_KEY_F75113 	0x50	/* Key to enable Super-I/O */
#define SIO_LD_GPIO_F81866  	0x06	/* GPIO logical device */
#define	SIO_UNLOCK_KEY_F81866 	0x87	/* Key to enable Super-I/O */

#define SIO_FINTEK_ID		0x1934	/* Manufacturer ID */
#define SIO_F75113_ID		0x1011	/* F75113 chipset ID */
#define SIO_F81801_ID		0x0723	/* F81801 chipset ID */
#define SIO_F81866_ID		0x1010	/* F81866 chipset ID */
#define SIO_F81966_ID		0x1502	/* F81966 chipset ID */

static int SIO_LD_GPIO;
static int SIO_UNLOCK_KEY;

enum chips { f75113, f81866d, f81966, f81801 };

static enum chips chip_num=-1;
module_param(chip_num, uint, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC(chip_num, "Super-I/O chips number (0=f75113, 1=f81866d, 2=f81966 3=f81801)");

static const char * const fxxx_names[] = {
	"f75113",
	"f81801",
	"f81866d",
	"f81966",
};

struct fxxx_sio {
	int addr;
	enum chips type;
};

struct fxxx_gpio_bank {
	struct gpio_chip chip;
	unsigned int regbase;
	struct fxxx_gpio_data *data;
};

struct fxxx_gpio_data {
	struct fxxx_sio *sio;
	int nr_bank;
	struct fxxx_gpio_bank *bank;
};

/*
 * Super-I/O functions.
 */

static inline int superio_inb(int base, int reg)
{
	outb(reg, base);
	return inb(base + 1);
}

static int superio_inw(int base, int reg)
{
	int val;

	outb(reg++, base);
	val = inb(base + 1) << 8;
	outb(reg, base);
	val |= inb(base + 1);

	return val;
}

static inline void superio_outb(int base, int reg, int val)
{
	outb(reg, base);
	outb(val, base + 1);
}

static inline int superio_enter(int base)
{
	/* Don't step on other drivers' I/O space by accident. */
	if (!request_muxed_region(base, 2, DRVNAME)) {
		pr_err(DRVNAME "I/O address 0x%04x already in use\n", base);
		return -EBUSY;
	}

	/* According to the datasheet the key must be send twice. */
	outb(SIO_UNLOCK_KEY, base);
	outb(SIO_UNLOCK_KEY, base);

	return 0;
}

static inline void superio_select(int base, int ld)
{
	outb(SIO_LDSEL, base);
	outb(ld, base + 1);
}

static inline void superio_exit(int base)
{
	outb(SIO_LOCK_KEY, base);
	release_region(base, 2);
}

/*
 * GPIO chip.
 */

static int fxxx_gpio_direction_in(struct gpio_chip *chip, unsigned offset);
static int fxxx_gpio_get(struct gpio_chip *chip, unsigned offset);
static int fxxx_gpio_direction_out(struct gpio_chip *chip,
				     unsigned offset, int value);
static void fxxx_gpio_set(struct gpio_chip *chip, unsigned offset, int value);

#define fxxx_GPIO_BANK(_base, _ngpio, _regbase)			\
	{								\
		.chip = {						\
			.label            = DRVNAME,			\
			.owner            = THIS_MODULE,		\
			.direction_input  = fxxx_gpio_direction_in,	\
			.get              = fxxx_gpio_get,		\
			.direction_output = fxxx_gpio_direction_out,	\
			.set              = fxxx_gpio_set,		\
			.base             = _base,			\
			.ngpio            = _ngpio,			\
			.can_sleep        = true,			\
		},							\
		.regbase = _regbase,					\
	}

#define gpio_dir(base) (base + 0)
#define gpio_data_out(base) (base + 1)
#define gpio_data_in(base) (base + 2)
/* Output mode register (0:open drain 1:push-pull). */
#define gpio_out_mode(base) (base + 3)

static struct fxxx_gpio_bank f75113_gpio_bank[] = {
	fxxx_GPIO_BANK(0, 8, 0x10),
	fxxx_GPIO_BANK(10, 8, 0x20),
	fxxx_GPIO_BANK(20, 8, 0x30),
	fxxx_GPIO_BANK(30, 8, 0x40),
	fxxx_GPIO_BANK(40, 8, 0x70),
};

static struct fxxx_gpio_bank f81801_gpio_bank[] = {
	fxxx_GPIO_BANK(0,  8, 0xF0),
	fxxx_GPIO_BANK(10, 8, 0xE0),
	fxxx_GPIO_BANK(20, 4, 0xD0),
	fxxx_GPIO_BANK(30, 8, 0xC0),
};

static struct fxxx_gpio_bank f81866_gpio_bank[] = {
	fxxx_GPIO_BANK(0, 8, 0xF0),
	fxxx_GPIO_BANK(10, 8, 0xE0),
	fxxx_GPIO_BANK(20, 8, 0xD0),
	fxxx_GPIO_BANK(30, 8, 0xC0),
	fxxx_GPIO_BANK(40, 8, 0xB0),
	fxxx_GPIO_BANK(50, 8, 0xA0),
	fxxx_GPIO_BANK(60, 8, 0x90),
	fxxx_GPIO_BANK(70, 8, 0x80),
	fxxx_GPIO_BANK(80, 8, 0x88),
};

static int fxxx_gpio_direction_in(struct gpio_chip *chip, unsigned offset)
{
	int err;
	struct fxxx_gpio_bank *bank =
		container_of(chip, struct fxxx_gpio_bank, chip);
	struct fxxx_sio *sio = bank->data->sio;
	u8 dir;

	err = superio_enter(sio->addr);
	if (err)
		return err;
	superio_select(sio->addr, SIO_LD_GPIO);

	dir = superio_inb(sio->addr, gpio_dir(bank->regbase));
	dir &= ~(1 << offset);
	superio_outb(sio->addr, gpio_dir(bank->regbase), dir);

	superio_exit(sio->addr);

	return 0;
}

static int fxxx_gpio_get(struct gpio_chip *chip, unsigned offset)
{
	int err;
	struct fxxx_gpio_bank *bank =
		container_of(chip, struct fxxx_gpio_bank, chip);
	struct fxxx_sio *sio = bank->data->sio;
	u8 dir, data;

	err = superio_enter(sio->addr);
	if (err)
		return err;
	superio_select(sio->addr, SIO_LD_GPIO);

	dir = superio_inb(sio->addr, gpio_dir(bank->regbase));
	dir = !!(dir & (1 << offset));
	if (dir)
		data = superio_inb(sio->addr, gpio_data_out(bank->regbase));
	else
		data = superio_inb(sio->addr, gpio_data_in(bank->regbase));

	superio_exit(sio->addr);

	return !!(data & 1 << offset);
}

static int fxxx_gpio_direction_out(struct gpio_chip *chip,
				     unsigned offset, int value)
{
	int err;
	struct fxxx_gpio_bank *bank =
		container_of(chip, struct fxxx_gpio_bank, chip);
	struct fxxx_sio *sio = bank->data->sio;
	u8 dir, data_out;

	err = superio_enter(sio->addr);
	if (err)
		return err;
	superio_select(sio->addr, SIO_LD_GPIO);

	data_out = superio_inb(sio->addr, gpio_data_out(bank->regbase));
	if (value)
		data_out |= (1 << offset);
	else
		data_out &= ~(1 << offset);
	superio_outb(sio->addr, gpio_data_out(bank->regbase), data_out);

	dir = superio_inb(sio->addr, gpio_dir(bank->regbase));
	dir |= (1 << offset);
	superio_outb(sio->addr, gpio_dir(bank->regbase), dir);

	superio_exit(sio->addr);

	return 0;
}

static void fxxx_gpio_set(struct gpio_chip *chip, unsigned offset, int value)
{
	int err;
	struct fxxx_gpio_bank *bank =
		container_of(chip, struct fxxx_gpio_bank, chip);
	struct fxxx_sio *sio = bank->data->sio;
	u8 data_out;

	err = superio_enter(sio->addr);
	if (err)
		return;
	superio_select(sio->addr, SIO_LD_GPIO);

	data_out = superio_inb(sio->addr, gpio_data_out(bank->regbase));
	if (value)
		data_out |= (1 << offset);
	else
		data_out &= ~(1 << offset);
	superio_outb(sio->addr, gpio_data_out(bank->regbase), data_out);

	superio_exit(sio->addr);
}

/*
 * Platform device and driver.
 */

static int fxxx_gpio_probe(struct platform_device *pdev)
{
	int err;
	int i;
	struct fxxx_sio *sio = pdev->dev.platform_data;
	struct fxxx_gpio_data *data;

	data = devm_kzalloc(&pdev->dev, sizeof(*data), GFP_KERNEL);
	if (!data)
		return -ENOMEM;

	switch (sio->type) {
	case f75113:
		data->nr_bank = ARRAY_SIZE(f75113_gpio_bank);
		data->bank = f75113_gpio_bank;
		break;
	case f81801:
		data->nr_bank = ARRAY_SIZE(f81801_gpio_bank);
		data->bank = f81801_gpio_bank;
		break;
	case f81866d:
		data->nr_bank = ARRAY_SIZE(f81866_gpio_bank);
		data->bank = f81866_gpio_bank;
		break;
	case f81966:
		data->nr_bank = ARRAY_SIZE(f81866_gpio_bank);
		data->bank = f81866_gpio_bank;
		break;
	default:
		return -ENODEV;
	}
	data->sio = sio;

	platform_set_drvdata(pdev, data);

	/* For each GPIO bank, register a GPIO chip. */
	for (i = 0; i < data->nr_bank; i++) {
		struct fxxx_gpio_bank *bank = &data->bank[i];
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 5, 0)
		bank->chip.dev = &pdev->dev;
#else
		bank->chip.parent = &pdev->dev;
#endif
		bank->data = data;

		err = gpiochip_add(&bank->chip);
		if (err) {
			dev_err(&pdev->dev,
				"Failed to register gpiochip %d: %d\n",
				i, err);
			goto err_gpiochip;
		}
	}

	return 0;

err_gpiochip:
	for (i = i - 1; i >= 0; i--) {
		struct fxxx_gpio_bank *bank = &data->bank[i];
		gpiochip_remove(&bank->chip);
	}

	return err;
}

static int fxxx_gpio_remove(struct platform_device *pdev)
{
	int i;
	struct fxxx_gpio_data *data = platform_get_drvdata(pdev);

	for (i = 0; i < data->nr_bank; i++) {
		struct fxxx_gpio_bank *bank = &data->bank[i];
		gpiochip_remove(&bank->chip);
	}
	
	pr_info(DRVNAME ":  %s\n", __func__);

	return 0;
}

static int __init fxxx_find(int addr, struct fxxx_sio *sio)
{
	int err;
	u16 devid;

	switch (chip_num) {
	case f75113:
		SIO_LD_GPIO = SIO_LD_GPIO_F75113;
		SIO_UNLOCK_KEY = SIO_UNLOCK_KEY_F75113;
		break;
	case f81801:
		SIO_LD_GPIO = SIO_LD_GPIO_F81866;
		SIO_UNLOCK_KEY = SIO_UNLOCK_KEY_F81866;
		break;
	case f81866d:
		SIO_LD_GPIO = SIO_LD_GPIO_F81866;
		SIO_UNLOCK_KEY = SIO_UNLOCK_KEY_F81866;
		break;
	case f81966:
		SIO_LD_GPIO = SIO_LD_GPIO_F81866;
		SIO_UNLOCK_KEY = SIO_UNLOCK_KEY_F81866;
		break;
	default:
		pr_info(DRVNAME ": Invalid for parameter 'chip_num' (0=f75113, 1=f81866d, 2=f81966, 3=f81801)");
		return -ENODEV;
	}
		
	err = superio_enter(addr);
	if (err)
		return err;

	err = -ENODEV;
	devid = superio_inw(addr, SIO_MANID);
	if (devid != SIO_FINTEK_ID) {
		pr_debug(DRVNAME ": Not a Fintek device at 0x%08x\n", addr);
		goto err;
	}

	devid = superio_inw(addr, SIO_DEVID);
	switch (devid) {
	case SIO_F75113_ID:
		sio->type = f75113;
		break;
	case SIO_F81801_ID:
		sio->type = f81801;
		break;
	case SIO_F81866_ID:
		sio->type = f81866d;
		break;
	case SIO_F81966_ID:
		sio->type = f81966;
		break;
	default:
		pr_info(DRVNAME ": Unsupported Fintek device 0x%04x\n", devid);
		goto err;
	}
	sio->addr = addr;
	err = 0;

	pr_info(DRVNAME ": Found %s at %#x, revision %d\n",
		fxxx_names[sio->type],
		(unsigned int) addr,
		(int) superio_inb(addr, SIO_DEVREV));

err:
	superio_exit(addr);
	return err;
}

static struct platform_device *fxxx_gpio_pdev;

static int __init
fxxx_gpio_device_add(const struct fxxx_sio *sio)
{
	int err;

	fxxx_gpio_pdev = platform_device_alloc(DRVNAME, -1);
	if (!fxxx_gpio_pdev)
		return -ENOMEM;

	err = platform_device_add_data(fxxx_gpio_pdev,
				       sio, sizeof(*sio));
	if (err) {
		pr_err(DRVNAME "Platform data allocation failed\n");
		goto err;
	}

	err = platform_device_add(fxxx_gpio_pdev);
	if (err) {
		pr_err(DRVNAME "Device addition failed\n");
		goto err;
	}

	return 0;

err:
	platform_device_put(fxxx_gpio_pdev);

	return err;
}

/*
 * Try to match a supported Fintek device by reading the (hard-wired)
 * configuration I/O ports. If available, then register both the platform
 * device and driver to support the GPIOs.
 */

static struct platform_driver fxxx_gpio_driver = {
	.driver = {
		.name	= DRVNAME,
	},
	.probe		= fxxx_gpio_probe,
	.remove		= fxxx_gpio_remove,
};

static int __init fxxx_gpio_init(void)
{
	int err;
	struct fxxx_sio sio;

	if (fxxx_find(0x2e, &sio) &&
	    fxxx_find(0x4e, &sio))
		return -ENODEV;

	//pr_info(DRVNAME ":  %s \n", __func__);
	err = platform_driver_register(&fxxx_gpio_driver);
	if (!err) {
		err = fxxx_gpio_device_add(&sio);
		if (err)
			platform_driver_unregister(&fxxx_gpio_driver);
	}

	pr_info(DRVNAME ":  %s \n", __func__);
	
	return err;
}
subsys_initcall(fxxx_gpio_init);

static void __exit fxxx_gpio_exit(void)
{
	platform_device_unregister(fxxx_gpio_pdev);
	platform_driver_unregister(&fxxx_gpio_driver);
}
module_exit(fxxx_gpio_exit);

MODULE_AUTHOR("Charlie Chan");
MODULE_DESCRIPTION("GPIO driver for Super-I/O chips F75113, F81801, F81866 and F81966");
MODULE_LICENSE("GPL");
