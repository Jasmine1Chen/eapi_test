#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h> 
#include <fcntl.h>  
#include <EApi.h>
#include <EApiCommon.h>

#define HAVE_LCM	0 //test only if you have the same lcm you can run set 1, else set 0.
#if HAVE_LCM
#define I2C_LCM_BASE	0x3C
#define LCM_PIC_BASE 	0x3F
#endif //HAVE_LCM


int ngpio_count = 0;

typedef struct EApiStringType_s{
	const EApiId_t Id  ;
  	const char *const   Desc;
}EApiStringType_t;

typedef struct EApiGpioType_s{
  	const EApiId_t  Id     ;
  	const char *const   Desc   ;
}EApiGpioType_t;

typedef struct EApiValueType_s{
  	const EApiId_t  Id     ;
  	const char *const   Desc   ;
}EApiValueType_t;

const EApiStringType_t EApiStrings[]={
  	{EAPI_ID_BOARD_MANUFACTURER_STR  , "Manufacturer"},
  	{EAPI_ID_BOARD_NAME_STR          , "Board Name"},
  	{EAPI_ID_BOARD_REVISION_STR      , "Board Revision"},
  	{EAPI_ID_BOARD_SERIAL_STR        , "Board Serial Number"},
  	{EAPI_ID_BOARD_BIOS_REVISION_STR , "Bios Revision"},
  	{EAPI_ID_BOARD_HW_REVISION_STR   , "HW Revision"},
  	{EAPI_ID_BOARD_PLATFORM_TYPE_STR , "Platform ID"},
  	{EAPI_ID_PRODUCT_NAME_STR	   , "Product Name"},
  	{EAPI_ID_BOARD_CPU_MODEL_NAME_STR      , "CPU Model Name"},
};

const EApiGpioType_t EApiGpioDevices[]={
//  {EAPI_ID_GPIO_BANK00   , TEXT("GPIO Bank0"      )},
  	{EAPI_ID_GPIO_GPIO00  , "GPIO0"},
  	{EAPI_ID_GPIO_GPIO01  , "GPIO1"},
  	{EAPI_ID_GPIO_GPIO02  , "GPIO2"},
  	{EAPI_ID_GPIO_GPIO03  , "GPIO3"},
  	{EAPI_ID_GPIO_GPIO04  , "GPIO4"},
  	{EAPI_ID_GPIO_GPIO05  , "GPIO5"},
  	{EAPI_ID_GPIO_GPIO06  , "GPIO6"},
  	{EAPI_ID_GPIO_GPIO07  , "GPIO7"},
};

const EApiValueType_t EApiValues[] = {
	{EAPI_ID_HWMON_CPU_TEMP , "CPU Temperature"},
	{EAPI_ID_HWMON_CHIPSET_TEMP, "Chipset Temperature"},
	{EAPI_ID_HWMON_SYSTEM_TEMP, "System Temperature"},	 
	{EAPI_ID_HWMON_VOLTAGE_VCORE, "VCORE"},
	{EAPI_ID_HWMON_VOLTAGE_2V5, "+2.5V"},
	{EAPI_ID_HWMON_VOLTAGE_3V3, "+3.3V"},
	{EAPI_ID_HWMON_VOLTAGE_VBAT, "VBAT"},
	{EAPI_ID_HWMON_VOLTAGE_5V, "+5V"},
	{EAPI_ID_HWMON_VOLTAGE_5VSB, "5VSB"},
	{EAPI_ID_HWMON_VOLTAGE_12V, "+12V"},
	{EAPI_ID_HWMON_VOLTAGE_VMEM, "VMEM"},
	{EAPI_ID_HWMON_VOLTAGE_3VSB, "3VSB"},
	{EAPI_ID_HWMON_FAN_CPU, "CPU FAN"},
	{EAPI_ID_HWMON_FAN_SYSTEM, "System FAN"},

};

typedef void (*Func)(void);

struct FuncItem {
	char *funcName;
	Func func;
};

int kbhit(void) {

	struct termios oldt, newt;
  	int ch;
  	int oldf;
 
  	tcgetattr(STDIN_FILENO, &oldt);
  	newt = oldt;
  	newt.c_lflag &= ~(ICANON | ECHO);
  	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  	oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  	fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
 
  	ch = getchar();
 
  	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  	fcntl(STDIN_FILENO, F_SETFL, oldf);
 
  	if(ch != EOF)
  	{
    		ungetc(ch, stdin);
    		return 1;
  	}
 
  	return 0;
}

#if HAVE_LCM
void i2c_r_w_test(uint32_t  Id) {

	uint8_t writebuffer = 0xF8;
	uint8_t testbuffer[2] = {0x40, 0xF8};
	uint32_t Cmd = 0x40;
	uint8_t Buffer = 0;
	
	uint32_t pMaxBlkLen;
	EApiStatus_t status;
	
		
	status = EApiI2CProbeDevice(Id, LCM_PIC_BASE);
	if(status != EAPI_STATUS_SUCCESS) {
		printf("EApiI2CProbeDevice is Fail error code is 0x%4X\n", status);
		return;
	}
	
	status = EApiI2CGetBusCap(Id, &pMaxBlkLen);
	if(status != EAPI_STATUS_SUCCESS) {
		printf("EApiI2CGetBusCap is Fail error code is 0x%4X\n", status);
		return;
	}else printf("i2c_r_w_test pMaxBlkLen is %u\n", pMaxBlkLen);
				
	status = EApiI2CWriteTransfer(Id, LCM_PIC_BASE, Cmd, &writebuffer, 1);
	if(status != EAPI_STATUS_SUCCESS) {
		printf("EApiI2CWriteTransfer is Fail error code is 0x%4X\n", status);
		return;
	}
	
	status = EApiI2CReadTransfer(Id, LCM_PIC_BASE, Cmd, &Buffer, 1, 1);
	if(status != EAPI_STATUS_SUCCESS) {
		printf("EApiI2CReadTransfer is Fail error code is 0x%4X\n", status);
		return;
	}
	
	printf("writebuffer data is 0x%02X, Buffer data is 0x%02X\n", writebuffer, Buffer);
		
	if(writebuffer != Buffer) {
		printf("EApiI2CWriteTransfer write data not equal EApiI2CReadTransfer read data\n");
		return;
	}
				
	printf("EApiI2CWriteTransfer write data equal EApiI2CReadTransfer read data\n");
	
	Buffer = 0;

	status = EApiI2CWriteReadRaw(Id, LCM_PIC_BASE, testbuffer, 2, &Buffer, 1, 1);
	
	if(status != EAPI_STATUS_SUCCESS) {
		printf("EApiI2CWriteReadRaw is Fail error code is 0x%4X\n", status);
		return;
	}

	printf("writebuffer data is 0x%02X, Buffer data is 0x%02X\n", writebuffer, Buffer);
	if(writebuffer != Buffer) {
		printf("EApiI2CWriteReadRaw write data not equal EApiI2CWriteReadRaw read data\n");
		return;
	}

	printf("EApiI2CWriteReadRaw write data equal EApiI2CWriteReadRaw read data\n");
	
	return;
}



void graphicwritelcm2(uint32_t  Id) {

	uint8_t position[512] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x0F,0x00,0x00,0x00,0x8C,0x8E,0x87,0x87,0xC7,0xC7,0xC7,0xC7,0xC7,0xCF,0xFF,0xFF,0xFE,0xFE,0x3C,0x00,0x06,0x07,0x83,0xC3,0xE3,0xE3,0xE3,0xE3,0xC3,0xC7,0xFF,0x03,0x03,0x03,0x03,0xE3,0xE3,0xE3,0x03,0x03,0x07,0x0F,0xFF,0xFF,0x3F,0x0F,0x07,0x07,0x83,0xE3,0xE3,0xE3,0xE3,0x83,0x07,0x07,0x0F,0x3F,0xFF,0x0F,0x07,0x07,0x03,0x63,0x63,0x63,0x63,0xE3,0xC3,0xC7,0xFF,0xFF,0x0F,0x07,0x07,0x03,0x63,0x63,0x63,0x63,0xE3,0xC3,0xC7,0xFF,0xFF,0x03,0x03,0x03,0x03,0x63,0x63,0x63,0x63,0x63,0xE3,0xFF,0x03,0x03,0x03,0x03,0xE3,0xE3,0xE3,0x03,0x03,0x07,0x0F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x0F,0x00,0x00,0x3C,0x7E,0xFF,0xFF,0xFF,0xE3,0xE3,0xE1,0x61,0x61,0x31,0xF9,0xFF,0xFF,0xFF,0xFF,0xFF,0xE0,0xC0,0x80,0x80,0x07,0x0F,0x1F,0x1F,0x1F,0x1F,0x0F,0x8F,0xFF,0x00,0x00,0x00,0x00,0xF8,0xE0,0xC0,0x80,0x00,0x06,0x1F,0x3F,0x7F,0xF0,0xC0,0x80,0x80,0x07,0x1F,0x1F,0x1F,0x1F,0x07,0x80,0x80,0xC0,0xF0,0xFF,0x8F,0x0E,0x1C,0x1C,0x1C,0x18,0x18,0x18,0x00,0x80,0x81,0xE3,0xFF,0x8F,0x0E,0x1C,0x1C,0x1C,0x18,0x18,0x18,0x00,0x80,0x81,0xE3,0xFF,0x00,0x00,0x00,0x00,0x1C,0x1C,0x1C,0x1C,0x1C,0x1F,0xFF,0x00,0x00,0x00,0x00,0xF8,0xE0,0xC0,0x80,0x00,0x06,0x1F,0x3F,0x7F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
	uint8_t *data = position;
	
	int r, h; // r = row, h = horiz_block;
	uint8_t wbuffer = 0;

	for(r=0 ; r < 4 ; r++) {
		wbuffer = 0x04;
		EApiI2CWriteTransfer(Id, I2C_LCM_BASE, 0, &wbuffer, 1); //Set Lower Column
		wbuffer = 0x10;
		EApiI2CWriteTransfer(Id, I2C_LCM_BASE, 0, &wbuffer, 1); //Set Higher Column
		wbuffer = 0xB0+r;
		EApiI2CWriteTransfer(Id, I2C_LCM_BASE, 0, &wbuffer, 1);
	

		for(h=0; h < 4; h++) {
			EApiI2CWriteTransfer(Id, I2C_LCM_BASE, 0x40, (data + (32 * h) + (128 * r)), 32); 
		}	

	}

	return;
}


void graphicwritelcm(uint32_t  Id, uint8_t *data) {

	int r, h; // r = row, h = horiz_block;
	uint8_t wbuffer = 0;

	for(r=0 ; r < 4 ; r++) {
		wbuffer = 0x04;
		EApiI2CWriteTransfer(Id, I2C_LCM_BASE, 0, &wbuffer, 1); //Set Lower Column
		wbuffer = 0x10;
		EApiI2CWriteTransfer(Id, I2C_LCM_BASE, 0, &wbuffer, 1); //Set Higher Column
		wbuffer = 0xB0+r;
		EApiI2CWriteTransfer(Id, I2C_LCM_BASE, 0, &wbuffer, 1);
	

		for(h=0; h < 4; h++) {
			EApiI2CWriteTransfer(Id, I2C_LCM_BASE, 0x40, (data + (32 * h) + (128 * r)), 32); 
		}	
	}

	return;
}


void lcm_init(uint32_t  Id) {

	uint8_t wbuffer = 0xAE;
	
	EApiI2CWriteTransfer(Id, I2C_LCM_BASE, 0, &wbuffer, 1);
	wbuffer = 0xAD;
	EApiI2CWriteTransfer(Id, I2C_LCM_BASE, 0, &wbuffer, 1);
	wbuffer = 0x8E;
	EApiI2CWriteTransfer(Id, I2C_LCM_BASE, 0, &wbuffer, 1);
	wbuffer = 0xA8;
	EApiI2CWriteTransfer(Id, I2C_LCM_BASE, 0, &wbuffer, 1);
	wbuffer = 0x1F;
	EApiI2CWriteTransfer(Id, I2C_LCM_BASE, 0, &wbuffer, 1);
	wbuffer = 0xD3;
	EApiI2CWriteTransfer(Id, I2C_LCM_BASE, 0, &wbuffer, 1);
	wbuffer = 0x00;
	EApiI2CWriteTransfer(Id, I2C_LCM_BASE, 0, &wbuffer, 1);
	wbuffer = 0x04;
	EApiI2CWriteTransfer(Id, I2C_LCM_BASE, 0, &wbuffer, 1);
	wbuffer = 0x10;
	EApiI2CWriteTransfer(Id, I2C_LCM_BASE, 0, &wbuffer, 1);
	wbuffer = 0x40;
	EApiI2CWriteTransfer(Id, I2C_LCM_BASE, 0, &wbuffer, 1);
	wbuffer = 0x20;
	EApiI2CWriteTransfer(Id, I2C_LCM_BASE, 0, &wbuffer, 1);
	wbuffer = 0x00;
	EApiI2CWriteTransfer(Id, I2C_LCM_BASE, 0, &wbuffer, 1);
	wbuffer = 0xA6;
	EApiI2CWriteTransfer(Id, I2C_LCM_BASE, 0, &wbuffer, 1);
	wbuffer = 0xDB;
	EApiI2CWriteTransfer(Id, I2C_LCM_BASE, 0, &wbuffer, 1);
	wbuffer = 0x20;
	EApiI2CWriteTransfer(Id, I2C_LCM_BASE, 0, &wbuffer, 1);
	wbuffer = 0xA4;
	EApiI2CWriteTransfer(Id, I2C_LCM_BASE, 0, &wbuffer, 1);
	wbuffer = 0xB1;
	EApiI2CWriteTransfer(Id, I2C_LCM_BASE, 0, &wbuffer, 1);
	wbuffer = 0x80;
	EApiI2CWriteTransfer(Id, I2C_LCM_BASE, 0, &wbuffer, 1);
	wbuffer = 0xD5;	
	EApiI2CWriteTransfer(Id, I2C_LCM_BASE, 0, &wbuffer, 1);
	wbuffer = 0xF0;
	EApiI2CWriteTransfer(Id, I2C_LCM_BASE, 0, &wbuffer, 1);
	wbuffer = 0xD8;
	EApiI2CWriteTransfer(Id, I2C_LCM_BASE, 0, &wbuffer, 1);
	wbuffer = 0x05;
	EApiI2CWriteTransfer(Id, I2C_LCM_BASE, 0, &wbuffer, 1);
	wbuffer = 0xA1;
	EApiI2CWriteTransfer(Id, I2C_LCM_BASE, 0, &wbuffer, 1);
	wbuffer = 0xC8;
	EApiI2CWriteTransfer(Id, I2C_LCM_BASE, 0, &wbuffer, 1);
	wbuffer = 0xDA;
	EApiI2CWriteTransfer(Id, I2C_LCM_BASE, 0, &wbuffer, 1);
	wbuffer = 0x12;
	EApiI2CWriteTransfer(Id, I2C_LCM_BASE, 0, &wbuffer, 1);
	wbuffer = 0xD9;
	EApiI2CWriteTransfer(Id, I2C_LCM_BASE, 0, &wbuffer, 1);
	wbuffer = 0xF2;
	EApiI2CWriteTransfer(Id, I2C_LCM_BASE, 0, &wbuffer, 1);
	wbuffer = 0xFF;
	EApiI2CWriteTransfer(Id, I2C_LCM_BASE, 0, &wbuffer, 1);
	wbuffer = 0xAF;
	EApiI2CWriteTransfer(Id, I2C_LCM_BASE, 0, &wbuffer, 1);

	return;
}


void i2cMenu (void) {

	uint32_t pMaxBlkLen;
	EApiStatus_t status;
	uint8_t position[512];
	uint8_t *data = position;
	
	status = EApiI2CProbeDevice(EAPI_ID_I2C_EXTERNAL, I2C_LCM_BASE);
	if(status != EAPI_STATUS_SUCCESS) {
		printf("EApiI2CProbeDevice is Fail error code is 0x%4X\n", status);
		return;
	}
	
	status = EApiI2CGetBusCap(EAPI_ID_I2C_EXTERNAL, &pMaxBlkLen);
	if(status != EAPI_STATUS_SUCCESS) {
		printf("EApiI2CGetBusCap is Fail error code is 0x%4X\n", status);
		return;
	}else printf("pMaxBlkLen is %u\n", pMaxBlkLen);
	
	
	lcm_init(EAPI_ID_I2C_EXTERNAL);
	memset(position, 0, 512);
	graphicwritelcm(EAPI_ID_I2C_EXTERNAL, data);

	memset(position, 0xFF, 512);
	graphicwritelcm(EAPI_ID_I2C_EXTERNAL, data);
	graphicwritelcm2(EAPI_ID_I2C_EXTERNAL);
		
	i2c_r_w_test(EAPI_ID_I2C_EXTERNAL);
	
	return;

}
#endif //HAVE_LCM


void setbktbn (void) {

	uint32_t Bright;
	uint32_t lvds;
	
RE_SETLVDSBRIGHT:

	printf("Please choice which lvds to Set Brightness (%d) %s ~ (%d) %s: ", EAPI_ID_BACKLIGHT_1, "PTN3460", EAPI_ID_BACKLIGHT_2, "AD5247");
	scanf("%u",&lvds);
	if(lvds > EAPI_ID_BACKLIGHT_2) {
		printf("The choice Invild, Please choice again\n");	
		goto RE_SETLVDSBRIGHT;	
	}	
	
RE_SETBRIGHT:
	if(lvds == EAPI_ID_BACKLIGHT_1) {
		printf("PTN3460 Set Brightness value %u~%u: ", EAPI_BACKLIGHT_SET_DIMEST, EAPI_BACKLIGHT_SET_BRIGHTEST);
		scanf("%u",&Bright);
		if((Bright > EAPI_BACKLIGHT_SET_BRIGHTEST)) {
			printf("The Brightness value is Invild, Please Input again\n");	
			goto RE_SETBRIGHT;	
		}		
	}else {
		printf("AD5247 Set Brightness value 0~127: ");
		scanf("%u",&Bright);
		if((Bright > 127)) {
			printf("The Brightness value is Invild, Please Input again\n");	
			goto RE_SETBRIGHT;	
		}	
	}
	
	EApiStatus_t status;
	status = EApiVgaSetBacklightBrightness(lvds, Bright);
	if(status != EAPI_STATUS_SUCCESS) {
    		printf("%s Set Backlight Brightness Fail error code is 0x%4X\n",lvds == 0? "PTN3460":"AD5247", status);
    		return;
  	} 
	printf("%s Set Backlight Brightness Successful\n", lvds == 0? "PTN3460":"AD5247");
	return;	

}

void getbktbn (void) {

	uint32_t pBright;
	uint32_t lvds;
	EApiStatus_t status;
	
RE_GETLVDSBRIGHT:

	printf("Please choice which lvds to Get Brightness (%d) %s ~ (%d) %s: ", EAPI_ID_BACKLIGHT_1, "PTN3460", EAPI_ID_BACKLIGHT_2, "AD5247");
	scanf("%u",&lvds);
	if(lvds > EAPI_ID_BACKLIGHT_2) {
		printf("The choice Invild, Please choice again\n");	
		goto RE_GETLVDSBRIGHT;	
	}		
	
	status = EApiVgaGetBacklightBrightness(lvds, &pBright);
	if(status != EAPI_STATUS_SUCCESS) {
    		printf("%s Get Backlight Brightness Fail error code is 0x%4X\n",lvds == 0? "PTN3460":"AD5247", status);
    		return;
  	} 
	printf("%s Backlight Brightness Current is %d\n",lvds == 0? "PTN3460":"AD5247", pBright);
	return;	

}

void setbktdn (void) {
	
	uint32_t Disable = EAPI_BACKLIGHT_SET_OFF;
	uint32_t lvds;
	EApiStatus_t status;
	
RE_SETBKTDN:
	
	printf("Please choice which lvds to Set Backlight Disable (%d) %s ~ (%d) %s: ", EAPI_ID_BACKLIGHT_1, "PTN3460", EAPI_ID_BACKLIGHT_2, "AD5247");
	scanf("%u",&lvds);
	if(lvds > EAPI_ID_BACKLIGHT_2) {
		printf("The choice Invild, Please choice again\n");	
		goto RE_SETBKTDN;	
	}	
	
	
	status = EApiVgaSetBacklightEnable(lvds, Disable);
	if(status != EAPI_STATUS_SUCCESS) {
    		printf("%s Set Backlight Disable Fail error code is 0x%4X\n", lvds == 0? "PTN3460":"AD5247", status);
    		return;
  	} 

	printf("%s Set Backlight Disable Successful\n", lvds == 0? "PTN3460":"AD5247");
	return;
}

void setbkten (void) {
	
	uint32_t Enable = EAPI_BACKLIGHT_SET_ON;
	uint32_t lvds;
	EApiStatus_t status;

RE_SETBKTEN:
	
	printf("Please choice which lvds to Set Backlight Enable (%d) %s ~ (%d) %s: ", EAPI_ID_BACKLIGHT_1, "PTN3460", EAPI_ID_BACKLIGHT_2, "AD5247");
	scanf("%u",&lvds);
	if(lvds > EAPI_ID_BACKLIGHT_2){
		printf("The choice Invild, Please choice again\n");	
		goto RE_SETBKTEN;	
	}	


	status = EApiVgaSetBacklightEnable(lvds, Enable);
	if(status != EAPI_STATUS_SUCCESS) {
    		printf("%s Set Backlight Enable Fail error code is 0x%4X\n", lvds == 0? "PTN3460":"AD5247", status);
    		return;
  	} 

	printf("%s Set Backlight Enable Successful\n", lvds == 0? "PTN3460":"AD5247");
	return;
}

void getbkten (void) {

	uint32_t pEnable, lvds;
	EApiStatus_t status;

RE_GETBKTEN:
	
	printf("Please choice which lvds to Get Backlight Enable Status (%d) %s ~ (%d) %s: ", EAPI_ID_BACKLIGHT_1, "PTN3460", EAPI_ID_BACKLIGHT_2, "AD5247");
	scanf("%u",&lvds);
	if(lvds > EAPI_ID_BACKLIGHT_2) {
		printf("The choice Invild, Please choice again\n");	
		goto RE_GETBKTEN;	
	}	
	
	
	
	status = EApiVgaGetBacklightEnable(lvds, &pEnable);
	if(status != EAPI_STATUS_SUCCESS) {
    		printf("%s EApiVgaGetBacklightEnable error code is 0x%4X\n", lvds == 0? "PTN3460":"AD5247", status);
    		return;
  	} 
	printf("%s Backlight Enable statsus is %s\n", lvds == 0? "PTN3460":"AD5247", pEnable == 0? "EAPI_BACKLIGHT_SET_ON":"EAPI_BACKLIGHT_SET_OFF");
	return;	

}

struct FuncItem blcList[] ={
	{"Get Backlight Enable Status", getbkten,},
	{"Set Backlight Enable", setbkten,},
	{"Set Backlight Disable", setbktdn,},
	{"Get Backlight Brightness", getbktbn,},
	{"Set Backlight Brightness", setbktbn,},
	
};

void blcMenu(void) {
	int i;
	int number = sizeof(blcList)/sizeof(blcList[0]);
	int choice = 99;
		
	while(choice) {
		printf("====================Backlight Control MENU===============\n");
		printf("(0) Exit\n");

		for(i=0; i<number; i++)
			printf("(%d) %s\n", i+1, blcList[i].funcName);
		printf("============================================\n");
		printf("select: ");
		scanf("%d", &choice);

		if(choice<0 || choice>number) {
			printf("Error!!!\n");
			continue;
		} else if(choice==0) {
			system("clear");
			break;
		}
		blcList[choice-1].func();
	}	

	return;
}

void wtdMenu(void) {

	uint32_t pMaxDelay = 0;
	uint32_t pMaxEventTimeout = 0;
	uint32_t pMaxResetTimeout = 0;
	uint32_t timevalue=0;
	uint32_t timecount = 0;
	unsigned char key;
		
	EApiStatus_t status;
	
	status = EApiWDogGetCap(&pMaxDelay, &pMaxEventTimeout, &pMaxResetTimeout);
	
	if(status != EAPI_STATUS_SUCCESS) {
    		printf("EApiWDogGetCap error code is 0x%4X\n", status);
    		return;
  	} 
  	
  	printf("pMaxDelay is %u, pMaxEventTimeout is %u, pMaxResetTimeout is %u\n", pMaxDelay, pMaxEventTimeout, pMaxResetTimeout);
  	

RE_TIMEVALUE:
	
	printf("Please input time value %u~%u(msec): ", pMaxDelay, pMaxResetTimeout);
	scanf("%u",&timevalue);
	if((timevalue < pMaxDelay) || (timevalue > pMaxResetTimeout)) {
		printf("The time value is Invild, Please Input again\n");	
		goto RE_TIMEVALUE;	
	}		

	status = EApiWDogStart(pMaxDelay , pMaxEventTimeout, timevalue);
	if(status != EAPI_STATUS_SUCCESS) return; 
	timecount = timevalue;
	while(1) {
		
		sleep(1);
		timecount -= 1000;
		system("clear");
		printf("\n==========================================================\n");
		printf("\n");
		printf("System will reboot %d msec\n", timecount);
		printf("\n\n");
		printf("Press 'q' to stop count\n");
		printf("Press 'r' to reset count\n");
		printf("\n==========================================================\n");
			
		if(kbhit()) {
			key = getchar();
			
			if(key == 0x71) {
				EApiWDogStop();
				break;
			} else if(key == 0x72) {
				EApiWDogTrigger();
				timecount = timevalue;
				continue;
			}	
		}
		
	}

	system("clear");
	
	return;
}

void hwmMenu(void) {

	EApiStatus_t StatusCode;
  	uint32_t pValue;
  	unsigned i;
  	float voltage;
  	int temperature;
  	
  	printf("\n======================Hardware Monitor MENU====================================\n");
	printf("\n");
	for(i=0;i<ARRAY_SIZE(EApiValues);i++){
		StatusCode = EApiBoardGetValue(EApiValues[i].Id, &pValue);
		if(StatusCode != EAPI_STATUS_SUCCESS) {
			switch(StatusCode) {
				case EAPI_STATUS_UNSUPPORTED:
					printf("%s:	%s\n",EApiValues[i].Desc, "EAPI_STATUS_UNSUPPORTED");
				break;
				case EAPI_STATUS_ERROR:
					printf("%s:	%s\n",EApiValues[i].Desc, "EAPI_STATUS_ERROR");
				break; 
			}
			
		} else {
			
			switch(EApiValues[i].Id) {
				case EAPI_ID_HWMON_CPU_TEMP:
					temperature = (int) (pValue / 1000);
					printf("%s:	%d oC\n",EApiValues[i].Desc, temperature);
				break;
				case EAPI_ID_HWMON_CHIPSET_TEMP:
					temperature = (int) (pValue / 1000);
					printf("%s:	%d oC\n",EApiValues[i].Desc, temperature);
				break;
				case EAPI_ID_HWMON_SYSTEM_TEMP:
					temperature = (int) (pValue / 1000);
					printf("%s:	%d oC\n",EApiValues[i].Desc, temperature);
				break;
				case EAPI_ID_HWMON_VOLTAGE_VCORE:
					voltage = (((float) pValue) / 1000);
					printf("%s:	%.3f V\n",EApiValues[i].Desc, voltage);
				break;
				case EAPI_ID_HWMON_VOLTAGE_3V3:
					voltage = (((float) pValue) / 1000);
					printf("%s:	%.3f V\n",EApiValues[i].Desc, voltage);
				break;
				case EAPI_ID_HWMON_VOLTAGE_VBAT:
					voltage = (((float) pValue) / 1000);
					printf("%s:	%.3f V\n",EApiValues[i].Desc, voltage);
				break;
				case EAPI_ID_HWMON_VOLTAGE_5V:
					voltage = (((float) pValue) / 1000);
					printf("%s:	%.3f V\n",EApiValues[i].Desc, voltage);
				break;
				case EAPI_ID_HWMON_VOLTAGE_5VSB:
					voltage = (((float) pValue) / 1000);
					printf("%s:	%.3f V\n",EApiValues[i].Desc, voltage);
				break;
				case EAPI_ID_HWMON_VOLTAGE_12V:
					voltage = (((float) pValue) / 1000);
					printf("%s:	%.3f V\n",EApiValues[i].Desc, voltage);
				break;
				case EAPI_ID_HWMON_VOLTAGE_VMEM:
					voltage = (((float) pValue) / 1000);
					printf("%s:	%.3f V\n",EApiValues[i].Desc, voltage);
				break;
				case EAPI_ID_HWMON_VOLTAGE_3VSB:
					voltage = (((float) pValue) / 1000);
					printf("%s:	%.3f V\n",EApiValues[i].Desc, voltage);
				break;
				case EAPI_ID_HWMON_FAN_CPU:
					printf("%s:	%d rpm\n",EApiValues[i].Desc, pValue);
				break;
					
			}
		}
			
	}
	
	printf("\n\n");
	printf("\n==========================================================\n");
			
	return;
}


void getgpiolevel(void) {

	uint32_t pLevel;
	int sel;
	EApiStatus_t StatusCode;
	
	
selgetgpiolevel:
	
	printf("Please Select which pin to get level\n");
	printf("Range: (0 - %d): ", ngpio_count - 1);
	scanf("%d", &sel);
	if((sel < 0) || (sel > ngpio_count - 1)) goto selgetgpiolevel; 
	
	StatusCode = EApiGPIOGetLevel(EApiGpioDevices[sel].Id, EAPI_GPIO_GPIO_BITMASK, &pLevel);
	
	if(StatusCode == EAPI_STATUS_SUCCESS) printf("%s level is %s\n", EApiGpioDevices[sel].Desc, pLevel == 1? "High":"Low" ); 
	else printf("%s Get level Fail\n", EApiGpioDevices[sel].Desc);
	
	return;

}

void setGpoL(void) {

	uint32_t direction;
	int sel;
	EApiStatus_t StatusCode;
	
selsetGpoL:
	
	printf("Please Select which pin to Set To Low\n");
	printf("Range: (0 - %d): ", ngpio_count - 1);
	scanf("%d", &sel);
	if((sel < 0) || (sel > ngpio_count - 1)) goto selsetGpoL; 
	
	StatusCode = EApiGPIOGetDirection(EApiGpioDevices[sel].Id, EAPI_GPIO_GPIO_BITMASK, &direction);
	if(StatusCode == EAPI_STATUS_SUCCESS) {
		if(direction == 1) {
			printf("%s Set to Low Fail, direction is %s\n", EApiGpioDevices[sel].Desc, direction==1? "Input":"Output");
			return;
		}
	} else {
		printf("%s Set to Low Fail\n", EApiGpioDevices[sel].Desc);
		return;
	}
	
	StatusCode = EApiGPIOSetLevel(EApiGpioDevices[sel].Id, EAPI_GPIO_GPIO_BITMASK, EAPI_GPIO_LOW);
    	
    	if(StatusCode == EAPI_STATUS_SUCCESS) printf("%s Set to Low Successful\n", EApiGpioDevices[sel].Desc);
			
	else printf("%s Set to Low Fail\n", EApiGpioDevices[sel].Desc);
	
	return;

}

void setGpoH(void) {

	uint32_t direction;
	int sel;
	EApiStatus_t StatusCode;
	
selsetGpoH:
	
	printf("Please Select which pin to Set To High\n");
	printf("Range: (0 - %d): ", ngpio_count - 1);
	scanf("%d", &sel);
	if((sel < 0) || (sel > ngpio_count - 1)) goto selsetGpoH; 
	
	StatusCode = EApiGPIOGetDirection(EApiGpioDevices[sel].Id, EAPI_GPIO_GPIO_BITMASK, &direction);
	if(StatusCode == EAPI_STATUS_SUCCESS) {
		if(direction == 1) {
			printf("%s Set to High Fail, direction is %s\n",EApiGpioDevices[sel].Desc, direction==1? "Input":"Output");
			return;
		}
	} else {
		printf("%s Set to High Fail\n", EApiGpioDevices[sel].Desc);
		return;
	}		
	
	StatusCode = EApiGPIOSetLevel(EApiGpioDevices[sel].Id, EAPI_GPIO_GPIO_BITMASK, EAPI_GPIO_HIGH);
    	
    	if(StatusCode == EAPI_STATUS_SUCCESS) printf("%s Set to High Successful\n", EApiGpioDevices[sel].Desc);
			
	else printf("%s Set to High Fail\n", EApiGpioDevices[sel].Desc);
	 
	return;


}


void sgpioin(void) {

	int sel;
	EApiStatus_t StatusCode;
		
selsgpioin:
	
	printf("Please Select which pin to set input\n");
	printf("Range: (0 - %d): ", ngpio_count - 1);
	scanf("%d", &sel);
	if((sel < 0) || (sel > ngpio_count - 1)) goto selsgpioin; 
	
	StatusCode = EApiGPIOSetDirection(EApiGpioDevices[sel].Id, EAPI_GPIO_GPIO_BITMASK, EAPI_GPIO_INPUT);
    	
    	if(StatusCode == EAPI_STATUS_SUCCESS) printf("%s Set direction to input Success\n", EApiGpioDevices[sel].Desc);
	else printf("%s Set direction to input Fail\n", EApiGpioDevices[sel].Desc);
	
	return;

}


void sgpiout(void) {

	int sel;
	EApiStatus_t StatusCode;
	
selsgpiout:
	
	printf("Please Select which pin to set output\n");
	printf("Range: (0 - %d): ", ngpio_count - 1);
	scanf("%d", &sel);
	if((sel < 0) || (sel > ngpio_count - 1)) goto selsgpiout; 
	
	StatusCode = EApiGPIOSetDirection(EApiGpioDevices[sel].Id, EAPI_GPIO_GPIO_BITMASK, EAPI_GPIO_OUTPUT);
	
	if(StatusCode == EAPI_STATUS_SUCCESS)  printf("%s Set direction to output Success\n", EApiGpioDevices[sel].Desc); 
	else printf("%s Set direction to output Fail\n", EApiGpioDevices[sel].Desc);
	
	return;
}


void getgpiodir(void) {

	int sel;
	EApiStatus_t StatusCode;
	uint32_t direction;
		
selgetgpiodir:
	
	printf("Please Select which pin to Get direction\n");
	printf("Range: (0 - %d): ", ngpio_count - 1);
	scanf("%d", &sel);
	if((sel < 0) || (sel > ngpio_count - 1)) goto selgetgpiodir;
	
	StatusCode = EApiGPIOGetDirection(EApiGpioDevices[sel].Id, EAPI_GPIO_GPIO_BITMASK, &direction);
	
	if(StatusCode == EAPI_STATUS_SUCCESS) printf("%s Get direction is %s\n", EApiGpioDevices[sel].Desc, direction==1? "Input":"Output"); 
	else printf("%s Get direction Fail\n", EApiGpioDevices[sel].Desc);
	 
	return;

}

void showAllGpio(void) {

	uint32_t direction;
	uint32_t pLevel;
	EApiStatus_t StatusCode;

	for(int i=0; i < ngpio_count; i++) {
	
		StatusCode = EApiGPIOGetDirection(EApiGpioDevices[i].Id, EAPI_GPIO_GPIO_BITMASK, &direction);
	
		if(StatusCode == EAPI_STATUS_SUCCESS) printf("%s Get direction is %s\n", EApiGpioDevices[i].Desc, direction==1? "Input":"Output"); 
		else printf("%s Get direction Fail the error code is 0x%4X\n", EApiGpioDevices[i].Desc, StatusCode);
	
		StatusCode = EApiGPIOGetLevel(EApiGpioDevices[i].Id, EAPI_GPIO_GPIO_BITMASK, &pLevel);
	
		if(StatusCode == EAPI_STATUS_SUCCESS) printf("%s level is %s\n", EApiGpioDevices[i].Desc, pLevel == 1? "High":"Low" ); 
		else printf("%s Get level Fail the error code is 0x%4x\n", EApiGpioDevices[i].Desc, StatusCode);
	}
	
	return;

}

void showAllGpiodircaps(void) {

	uint32_t pInputs = 0, pOutputs=0;
	EApiStatus_t StatusCode;
	
	for(int i=0; i < ngpio_count; i++) {
	
		StatusCode = EApiGPIOGetDirectionCaps(EApiGpioDevices[i].Id, &pInputs, &pOutputs);
	
		if(StatusCode == EAPI_STATUS_SUCCESS) printf("%s Get direction caps pInput is %d, pOutput is %d\n", EApiGpioDevices[i].Desc, pInputs, pOutputs); 
		else printf("%s Get direction caps Fail the error code is 0x%4x\n", EApiGpioDevices[i].Desc, StatusCode);
	
	}
	
	return;


}

struct FuncItem gpioList[] ={
	{"Get GPIO pin direction", getgpiodir,},
	{"Show ALL GPIO status", showAllGpio,},
	{"Set GPIO pin to input", sgpioin,},
	{"Set GPIO pin to output", sgpiout,},
	{"Get GPIO pin level", getgpiolevel,},
	{"Set GPIO pin to Low" , setGpoL,},
	{"Set GPIO pin to High", setGpoH,},
	{"show ALL GPIO direction Caps", showAllGpiodircaps,},
};


void gpioMenu(void) {
	int i;
	int number = sizeof(gpioList)/sizeof(gpioList[0]);
	int choice = 99;
	uint32_t pCount;
	
	EApiStatus_t StatusCode;
	StatusCode = EApiGPIOGetCount(&pCount);
	if(StatusCode != EAPI_STATUS_SUCCESS) {
		printf("EApiGPIOGetCount not Successful the error code is 0x%4X\n", StatusCode);
    		return;
	
	}
	
	ngpio_count = (int) pCount; 
	//printf("ngpio_count is %d\n", ngpio_count);
	
	while(choice) {
		printf("====================GPIO MENU===============\n");
		printf("(0) Exit\n");

		for(i=0; i<number; i++)
			printf("(%d) %s\n", i+1, gpioList[i].funcName);
		printf("============================================\n");
		printf("select: ");
		scanf("%d", &choice);

		if(choice<0 || choice>number) {
			printf("Error!!!\n");
			continue;
		} else if(choice==0) {
			system("clear");
			break;
		}
		gpioList[choice-1].func();
	}	

	return;
}


void sysMenu(void) {

	EApiStatus_t StatusCode;
  	char * pBuffer=NULL;
  	uint32_t pBufferLen=100;
  	unsigned i;
  	pBuffer=(char *)malloc((pBufferLen) * sizeof(char));
  	
  	printf("\n======================System Information MENU====================================\n");
	printf("\n");
	for(i=0;i<ARRAY_SIZE(EApiStrings);i++){
		pBufferLen=100;
		
		StatusCode = EApiBoardGetStringA(EApiStrings[i].Id, pBuffer,&pBufferLen);
		if(StatusCode != EAPI_STATUS_SUCCESS) {
			printf("EApiBoardGetStringA error code is 0x%4X\n", StatusCode);
			goto sysMenu_err;
		}
		printf("%s:	%s\n",EApiStrings[i].Desc, pBuffer);
			
	}	
	printf("\n\n");
	printf("\n==========================================================\n");
			
	
sysMenu_err:	
	free(pBuffer);
	
  	return;
}

struct FuncItem funcList[]= {
		
	{"System infomation", sysMenu,},
	{"GPIO",	gpioMenu,},
	{"Hardware Monitor", hwmMenu,},
	{"WatchDog",	wtdMenu,},
	{"Backlight control", blcMenu,},
#if HAVE_LCM	
	{"I2C LCM Test", i2cMenu,},
#endif	
};


int main(void) {

	int i=0;
	int number = sizeof(funcList)/sizeof(funcList[0]);
	int choice = 99;

	
	EApiStatus_t status;
	
	status = EApiLibInitialize();
	
	if(status != EAPI_STATUS_SUCCESS) {
    		printf("EApiLibInitialize() not Successful the error code is 0x%4X\n", status);
    		exit(-1);
  	}

 	while(choice) {
		printf("==========================================================\n");
		printf("==================TestUtility  ================\n");
		printf("============Designed by Charlie Chan ===================\n");
		printf("==========================================================\n");
		printf("=======================Main MENU==========================\n");
		printf("(0) Exit\n");

		for(i=0; i<number; i++)
			printf("(%d) %s\n", i+1, funcList[i].funcName);

		printf("==========================================================\n");
		printf("select: ");
		scanf("%d", &choice);

		if(choice<0 || choice>number) {
			printf("Error!!!\n");
			continue;
		} else if(choice==0) {
			system("clear");
			EApiLibUnInitialize();
			break;
		} 
		

		funcList[choice-1].func();
		
	}
	
  	return 0;
}
