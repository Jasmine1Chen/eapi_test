/*
 *<KHeader>
 *+=========================================================================
 *I  Project Name: EApiDK Embedded Application Development Kit
 *+=========================================================================
 *I  $HeadURL$
 *+=========================================================================
 *I   Copyright: Copyright (c) 2002-2009, Kontron Embedded Modules GmbH
 *I      Author: John Kearney,                  John.Kearney@kontron.com
 *I
 *I     License: All rights reserved. This program and the accompanying
 *I              materials are licensed and made available under the
 *I              terms and conditions of the BSD License which
 *I              accompanies this distribution. The full text of the
 *I              license may be found at
 *I              http://opensource.org/licenses/bsd-license.php
 *I
 *I              THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "
 *I              AS IS" BASIS, WITHOUT WARRANTIES OR REPRESENTATIONS OF
 *I              ANY KIND, EITHER EXPRESS OR IMPLIED.
 *I
 *I Description: Auto Created for EApiEmulI2C.c
 *I
 *+-------------------------------------------------------------------------
 *I
 *I  File Name            : EApiEmulI2C.c
 *I  File Location        : lib
 *I  Last committed       : $Revision$
 *I  Last changed by      : $Author$
 *I  Last changed date    : $Date$
 *I  ID                   : $Id$
 *I
 *+=========================================================================
 *</KHeader>
 */

#include <EApiLib.h>
#include <stdio.h>
#include <EApiI2c-dev.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <common.h>
#include <hook_boardinfo.h>
#include <limits.h>
#include <hook_i2c.h> 

static unsigned I2CFound = 0;
static int i2cDescriptor = -1;
static int unknowId = 1;

#define I2C_DEV_PATH	"/sys/class/i2c-dev/"

EApiStatus_t i2c_probe_device(__IN uint32_t Id, __IN uint32_t Addr) {

	DBG("%s call\n", __func__);

	EApiStatus_t StatusCode = EAPI_STATUS_NOT_INITIALIZED;
	
	if (Id > 0xFFFF) {
		printf("%s Unrecognised I2C ID\n",__func__);
		EAPI_LIB_RETURN_ERROR(i2c_probe_device, EAPI_STATUS_UNSUPPORTED, "Unrecognised I2C ID");
	}
	FILE *file = fopen(BOARD_CONFIG_PATH, "r");
    	if(file != NULL) {
    		char *object[100];
    		char *type[100];
    		char *ref1[100];
    		char *ref2[100];
    		size_t counter = 0;
		char *line = NULL;
    		size_t length = 0;
    		char lines[NAME_MAX];
    		char buf[NAME_MAX];
    		//long size = 0;
    		char *p=NULL;
    		char i2c_check[NAME_MAX] = "";
    		char i2cdevicename[20] = "";
    		int ret = 1;
    		
    		while ((length = read_line(file, &line)) > 0 && counter < sizeof(object)) {
        		Category token_category = 0;
        		char *next_token = strtok(line, ":");
        		while (next_token != NULL) {
            			size_t token_length = strlen(next_token);
            			switch (token_category) {
                			case OBJECT:
                    				object[counter] = calloc(token_length, sizeof(char));
                    				memcpy(object[counter], next_token, token_length);
                    			break;
                			case TYPE:
                    				type[counter] = calloc(token_length, sizeof(char));
                    				memcpy(type[counter], next_token, token_length);
                    			break;
                    			case REF1:
                    				ref1[counter] = calloc(token_length, sizeof(char));
                    				memcpy(ref1[counter], next_token, token_length);
                    			break;
                			case REF2:
                    				ref2[counter] = calloc(token_length, sizeof(char));
                    				memcpy(ref2[counter], next_token, token_length);
                    			break;      
            			}
            			// Read the next token from the same string
            			next_token = strtok(NULL, ";");
            			token_category = next_category(token_category);
        		}
        		free(next_token);
        		free(line);
        		++counter;
    		}
    		fclose(file);
    		
    		strcpy(i2c_check, BOARD_I2C_PATH);
    		strcat(i2c_check, type[22]);
		i2c_check[strlen(i2c_check) - 1] = '\0';
		DBG("i2c_check is %s\n", i2c_check);
		
		I2CFound = direxist(BOARD_I2C_PATH);
		if (I2CFound == 0) {
        		DBGInfo("I2C driver not Found\n");
        		return StatusCode;
    		}
    		
    		DBG("I2C driver Found\n");
    		I2CFound = 1;
    		memset(i2c_check, 0, sizeof(i2c_check));
    		strcpy(i2c_check, "lspci | grep SMBus | awk '{print $1}' >> i2ctemp.txt");
    		DBG("i2c_check is %s\n", i2c_check);
    		system(i2c_check);
    		
    		file = fopen("i2ctemp.txt", "r");
		memset(buf, 0, NAME_MAX);
		memset(lines, 0, NAME_MAX);
		while (fgets(lines, sizeof(lines), file) != NULL) strcpy( buf, lines);
		p=strrchr(buf, '\n');/* search last space */
		*p = '\0';/* split at last space */
		
		fclose(file);
		system("rm -rf i2ctemp.txt");
		memset(i2c_check, 0, sizeof(i2c_check));
		strcpy(i2c_check, "ls -al ");
		strcat(i2c_check, I2C_DEV_PATH);
		strcat(i2c_check, " | grep ");
		strcat(i2c_check, buf);
		strcat(i2c_check, " | awk -F '->' '{print $1}' | awk '{print $9}' >> i2ctemp.txt");
		DBG("i2c_check is %s\n", i2c_check);
		system(i2c_check);		
	
		file = fopen("i2ctemp.txt", "r");
		memset(buf, 0, NAME_MAX);
		memset(lines, 0, NAME_MAX);
		while (fgets(lines, sizeof(lines), file) != NULL) strcpy( buf, lines);
		p=strrchr(buf, '\n');/* search last space */
		*p = '\0';/* split at last space */
		
		fclose(file);
		system("rm -rf i2ctemp.txt");
		memset(i2c_check, 0, sizeof(i2c_check));
		strcpy(i2c_check, "/dev/");
		strcat(i2c_check, buf);
		DBG("i2c_check is %s\n", i2c_check);
			
		snprintf(i2cdevicename, 19 , "/dev/i2c-%u", Id);
		DBG("i2cdevicename is %s\n", i2cdevicename);
		
		ret = strcmp(i2c_check, i2cdevicename);
		if(ret != 0) {
			unknowId = 1;
			printf("I2C device not Found\n");
        		return StatusCode;
		}
		unknowId = 0;
		
		if((Addr & 0x80) == 0x80){ //10bit addr
     			printf("%s 10Bit Address is not supported\n", __func__);
     			EAPI_LIB_RETURN_ERROR(i2c_probe_device, EAPI_STATUS_UNSUPPORTED, "10Bit Address is not supported");
       		
     		}
		i2cDescriptor = open(i2cdevicename,O_RDWR);
		if(i2cDescriptor < 0) {
			printf("Unrecognised i2c device %s\n",i2cdevicename);
        		snprintf(err,sizeof(err),"Unrecognised i2c device %s\n ",strerror(errno));
        		EAPI_LIB_RETURN_ERROR(i2c_probe_device, EAPI_STATUS_NOT_INITIALIZED, err);
    		}
		/* set slave address : set working device */
    		if(ioctl(i2cDescriptor, I2C_SLAVE, (Addr & 0x00ff)) < 0) {
        		printf("%s Cannot set i2c slave Address\n", __func__);
        		snprintf(err,sizeof(err),"Cannot set i2c slave ddress: %s\n",strerror(errno));
        		EAPI_LIB_RETURN_ERROR(i2c_probe_device, EAPI_STATUS_NOT_FOUND, err);
    		}
    		
    		StatusCode = EAPI_STATUS_SUCCESS;
    	}	

EAPI_LIB_ASSERT_EXIT    		
    	return StatusCode;

}


EApiStatus_t i2c_get_bus_cap(__IN uint32_t Id, __OUT uint32_t *pMaxBlkLen) {

	DBG("%s call\n", __func__);
	
	EApiStatus_t StatusCode = EAPI_STATUS_NOT_INITIALIZED;
	
	if (Id > 0xFFFF) {
		DBG("%s Unrecognised I2C ID\n",__func__);
		EAPI_LIB_RETURN_ERROR(i2c_get_bus_cap, EAPI_STATUS_UNSUPPORTED, "Unrecognised I2C ID");
	}
	
	if(unknowId == 1) {
		DBG("%s unknowId\n", __func__);
		EAPI_LIB_RETURN_ERROR(i2c_get_bus_cap, EAPI_STATUS_UNSUPPORTED, "unknowId");	
	}
	
	if (I2CFound == 0) {
        	DBG("I2C driver not Found\n");
        	return StatusCode;
    	}
    		
    	DBG("I2C driver Found\n");

	*pMaxBlkLen = (uint32_t)I2C_SMBUS_BLOCK_MAX;
    		
    	EAPI_LIB_RETURN_SUCCESS(i2c_get_bus_cap,""); 
	
EAPI_LIB_ASSERT_EXIT
	return StatusCode;
  		
}

/*
 *
 *
 *
 *      I2C
 *
 *
 *
 */
#define CHECK_I2C_FUNC( var, label ) \
    do { 	if(0 == (var & label)) { \
    fprintf(stderr, "\nError: " \
#label " function is required. Program halted.\n\n"); \
    exit(1); } \
    } while(0);

EApiStatus_t i2c_write_read(__IN uint32_t Id, __IN uint8_t Addr, __INOPT uint8_t *pWBuffer, __IN uint32_t WriteBCnt,
    __OUTOPT uint8_t *pRBuffer,__IN uint32_t RBufLen, __IN uint32_t ReadBCnt) {
    
    	DBG("%s call\n", __func__);
    
	EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
    	if (Id > 0xFFFF) {
    		printf("%s Unrecognised I2C ID\n", __func__);	
    		EAPI_LIB_RETURN_ERROR(i2c_write_read, EAPI_STATUS_UNSUPPORTED, "Unrecognised I2C ID");
	}
	
	if((WriteBCnt > (I2C_SMBUS_BLOCK_MAX+1)) || (ReadBCnt > (I2C_SMBUS_BLOCK_MAX+1))) {
		StatusCode = EAPI_STATUS_INVALID_BLOCK_LENGTH;
		return StatusCode;
	}
	
	
    	int funcs;
    	uint8_t index=0;
    	
     	if((Addr & 0x80) == 0x80){ //10bit addr
     		printf("10Bit Address is not supported\n");
     		EAPI_LIB_RETURN_ERROR(i2c_write_read, EAPI_STATUS_UNSUPPORTED, "10Bit Address is not supported");
       		
     	}
     	

    	/* get funcs list */
    	if(ioctl(i2cDescriptor, I2C_FUNCS, &funcs) < 0) {
        	printf("Unrecognised i2c funcs\n");	
        	snprintf(err,sizeof(err),"Unrecognised i2c funcs: %s\n",strerror(errno));
        	EAPI_LIB_RETURN_ERROR(i2c_write_read, EAPI_STATUS_NOT_FOUND, err);
    	}

    	/* check for req funcs */
    	CHECK_I2C_FUNC( funcs, I2C_FUNC_SMBUS_READ_BYTE );
    	CHECK_I2C_FUNC( funcs, I2C_FUNC_SMBUS_WRITE_BYTE );
    	CHECK_I2C_FUNC( funcs, I2C_FUNC_SMBUS_READ_BYTE_DATA );
    	CHECK_I2C_FUNC( funcs, I2C_FUNC_SMBUS_WRITE_BYTE_DATA );
    	CHECK_I2C_FUNC( funcs, I2C_FUNC_SMBUS_READ_WORD_DATA );
    	CHECK_I2C_FUNC( funcs, I2C_FUNC_SMBUS_WRITE_WORD_DATA );

    	/* set slave address : set working device */
    	if(ioctl(i2cDescriptor, I2C_SLAVE, (Addr & 0x00ff)) < 0) {
        	printf("%s Cannot set i2c slave Address\n", __func__);
        	snprintf(err,sizeof(err),"Cannot set i2c slave ddress: %s\n",strerror(errno));
        	EAPI_LIB_RETURN_ERROR(i2c_write_read, EAPI_STATUS_NOT_FOUND, err);
    	}
    	/*  write to the device */
    	if(WriteBCnt > 1) {
		index = *pWBuffer;
		WriteBCnt --; 
		pWBuffer += 1;
		i2c_smbus_write_i2c_block_data(i2cDescriptor, index, WriteBCnt, pWBuffer);
	}else {
		index = 0;
		i2c_smbus_write_byte(i2cDescriptor, pWBuffer);	
	}	
	
	/*read to the device */
	if(ReadBCnt > 1) {
		ReadBCnt --;
		i2c_smbus_read_i2c_block_data(i2cDescriptor, index, ReadBCnt, pRBuffer);	
	} else {
		*pRBuffer = i2c_smbus_read_byte(i2cDescriptor);
	}
	
	
     EAPI_LIB_ASSERT_EXIT
            return StatusCode;
}

EApiStatus_t i2c_read_transfer(__IN uint32_t Id, __IN uint32_t Addr, __IN uint32_t Cmd, __OUT uint8_t *pBuffer, __IN uint32_t BufLen, __IN uint32_t ByteCnt) {

		
	DBG("%s call\n", __func__);
    
	EApiStatus_t StatusCode = EAPI_STATUS_NOT_INITIALIZED;
	
	if (Id > 0xFFFF) {
    		printf("%s Unrecognised I2C ID\n", __func__);	
    		EAPI_LIB_RETURN_ERROR(i2c_read_transfer, EAPI_STATUS_UNSUPPORTED, "Unrecognised I2C ID");
	}
	
	if(unknowId == 1) {
		DBG("%s unknowId\n", __func__);
		EAPI_LIB_RETURN_ERROR(i2c_read_transfer, EAPI_STATUS_UNSUPPORTED, "unknowId");	
	}
	
	if (I2CFound == 0) {
        	DBG("I2C driver not Found\n");
        	return StatusCode;
    	}
    	
    	if((Addr & 0x80) == 0x80){ //10bit addr
     		printf("%s 10Bit Address is not supported\n", __func__);
     		EAPI_LIB_RETURN_ERROR(i2c_read_transfer, EAPI_STATUS_UNSUPPORTED, "10Bit Address is not supported");
       		
     	}
     	
     	/* set slave address : set working device */
    	if(ioctl(i2cDescriptor, I2C_SLAVE, (Addr & 0x00ff)) < 0) {
        	printf("%s Cannot set i2c slave Address\n", __func__);
        	snprintf(err,sizeof(err),"Cannot set i2c slave ddress: %s\n",strerror(errno));
        	EAPI_LIB_RETURN_ERROR(i2c_read_transfer, EAPI_STATUS_NOT_FOUND, err);
    	}
    	
    	if(ByteCnt > I2C_SMBUS_BLOCK_MAX) {
    		StatusCode = EAPI_STATUS_INVALID_BLOCK_LENGTH;
    		return StatusCode;
    	}

	if(ByteCnt > BufLen) {
		StatusCode = EAPI_STATUS_MORE_DATA;
		return StatusCode;
	
	}
	
	uint32_t value = 0;
	uint8_t index= (uint8_t)Cmd;
	uint8_t conunt = (uint8_t)ByteCnt;
	usleep(100000);
	
	if(ByteCnt > 1) i2c_smbus_read_i2c_block_data(i2cDescriptor, index, conunt, pBuffer);
	else {
		
		value = i2c_smbus_read_byte_data(i2cDescriptor, index);
		*pBuffer = (uint8_t)value;
		DBG("%s pBuffer is 0x%02X\n", __func__, *pBuffer);
	}
	
	StatusCode = EAPI_STATUS_SUCCESS;
		
EAPI_LIB_ASSERT_EXIT
            return StatusCode;	


}

EApiStatus_t i2c_write_transfer (__IN uint32_t Id, __IN uint32_t Addr, __IN uint32_t Cmd, __IN uint8_t *pBuffer, __IN uint32_t ByteCnt) {

	DBG("%s call\n", __func__);
    
	EApiStatus_t StatusCode = EAPI_STATUS_NOT_INITIALIZED;
	
    	if (Id > 0xFFFF) {
    		printf("%s Unrecognised I2C ID\n", __func__);	
    		EAPI_LIB_RETURN_ERROR(i2c_read_transfer, EAPI_STATUS_UNSUPPORTED, "Unrecognised I2C ID");
	}
	
	if(unknowId == 1) {
		printf("%s unknowId\n", __func__);
		EAPI_LIB_RETURN_ERROR(i2c_write_transfer, EAPI_STATUS_UNSUPPORTED, "unknowId");	
	}
	
	if (I2CFound == 0) {
        	DBG("I2C driver not Found\n");
        	return StatusCode;
    	}
    	
    	if((Addr & 0x80) == 0x80){ //10bit addr
     		printf("%s 10Bit Address is not supported\n", __func__);
     		EAPI_LIB_RETURN_ERROR(i2c_probe_device, EAPI_STATUS_UNSUPPORTED, "10Bit Address is not supported");
       		
     	}
     	
     	/* set slave address : set working device */
    	if(ioctl(i2cDescriptor, I2C_SLAVE, (Addr & 0x00ff)) < 0) {
        	printf("%s Cannot set i2c slave Address\n", __func__);
        	snprintf(err,sizeof(err),"Cannot set i2c slave ddress: %s\n",strerror(errno));
        	EAPI_LIB_RETURN_ERROR(i2c_write_transfer, EAPI_STATUS_NOT_FOUND, err);
    	}
     	
     	if(ByteCnt > 1) {
     		DBG("%s run i2c_smbus_write_block_data\n", __func__);
     		i2c_smbus_write_i2c_block_data(i2cDescriptor, (uint8_t)Cmd, ByteCnt, pBuffer);
     	}else {
     		DBG("%s run i2c_smbus_write_byte_data\n", __func__);
     		DBG("pBuffer is 0x%02X\n", *pBuffer);
     		i2c_smbus_write_byte_data(i2cDescriptor,(uint8_t)Cmd, *pBuffer);
     		
     	}
     	
     	StatusCode = EAPI_STATUS_SUCCESS;

EAPI_LIB_ASSERT_EXIT
            return StatusCode;

}

EApiStatus_t i2c_aaeon_deinit(void) {

	EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
	close(i2cDescriptor);
	
	StatusCode = EAPI_STATUS_SUCCESS;

	return StatusCode;
}

