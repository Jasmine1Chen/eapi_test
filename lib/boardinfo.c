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
 *I Description: Auto Created for EApiEmulBoardInfo.c
 *I
 *+-------------------------------------------------------------------------
 *I
 *I  File Name            : EApiEmulBoardInfo.c
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
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>

#include <hook_boardinfo.h>

//parse
#define BUF_SIZE 512

size_t next_buffer(const size_t buffer, const size_t count) {

	return (buffer + 1) % count;
}

size_t read_line(FILE *file, char **line) {

	if (file == NULL) return 0;
    	
    	size_t buffer_size = BUF_SIZE;
    	size_t start = 0;
    	char *buffers[2];
    	size_t buffer = 0;
    	size_t next = next_buffer(buffer, sizeof(buffers));
    	buffers[buffer] = calloc(buffer_size, sizeof(char));
    	buffers[next_buffer(buffer, sizeof(buffers))] = NULL;
    	while (1) {
        	char *result = fgets(buffers[buffer] + start, buffer_size - start, file);
        	if (result == NULL) break;
        	
        	// The line is longer than the buffer
        	if (buffers[buffer][buffer_size - 2] != 0 && buffers[buffer][buffer_size - 2] != '\n') {
            		if (buffers[next] != NULL)  free(buffers[next]);
            		
            		// Allocate the next buffer and copy this buffer's contents into it
            		buffers[next] = calloc(buffer_size * 2, sizeof(char));
            		if (!memcpy(buffers[next], buffers[buffer], buffer_size)) break;
            		
            		// Swap the buffers around
            		start = buffer_size - 1;
            		buffer_size *= 2;
            		buffer = next;
            		next = next_buffer(next, sizeof(buffers));
        	} else {
               		// The whole line was read
            		size_t length = strlen(buffers[buffer]) + 1;
            		// The final \n is replaced by 0, so the length (including 0)
            		// is decreased by 1
            		if (buffers[buffer][length - 1] == '\n') {
                		buffers[buffer][length - 1] = 0;
                		--length;
            		}
            		*line = calloc(length, sizeof(char));
            		if (memcpy(*line, buffers[buffer], length) == NULL) break;
           	 	
            		// Free the buffers used for reading in the line
            		if (buffers[buffer] != NULL) free(buffers[buffer]);
            		
            		if (buffers[next] != NULL) free(buffers[next]);
            		
            		return length - 1;
        	}
    	}
    	// Free the buffers used for reading in the line
    	if (buffers[buffer] != NULL) free(buffers[buffer]);
   	
    	if (buffers[next] != NULL) free(buffers[next]);
    	
    	return 0;
}

Category next_category(const Category category) {
    	
    	return (Category) ((category + 1) % 4);
}
//end parse

void file_open(char* path, char* info) {
	
	FILE *f = NULL;
  	char line[NAME_MAX];
  	char *p;
  	int retclose;

  	DBG("file_open path = %s\n", path);

  	f = fopen(path, "r");
  	if (f != NULL) {
    		retclose = 0;
    		char* res = fgets(line, sizeof(line), f);

    		if (res == NULL) {
      			fclose(f);
      			snprintf(err,sizeof(err),"Error during read operation: %s\n ",strerror(errno));
      			return;
    		} else {
      			retclose = fclose(f);
			if (retclose != 0 ) return;
      		}

    		p=strrchr(line, '\n');/* search last space */
    		*p = '\0';/* split at last space */
    		strcpy(info, line);
    		
    		DBG("file_open line = %s\n", line);
    		DBG("file_open info = %s\n", info);
    		
  	} else return;
  
}

void cpu_open(char *info) {

	system("cat /proc/cpuinfo | grep \"model name\" | awk -F ':' '{print $2}' | tail -n 1 >> temp.txt");
	FILE *input = NULL;
	input = fopen("temp.txt", "r");
	char *p=NULL;
	char buf[NAME_MAX];
	char line[NAME_MAX];
	
	memset(buf, 0, NAME_MAX);
	memset(line, 0, NAME_MAX);
	
	while (fgets(line, sizeof(line), input) != NULL) strcpy( buf, line);
	
	DBG("NAME_MAX is %d\n", NAME_MAX);
	
	DBG("%s\n", buf);
	
	fclose(input);
	
	p=strrchr(buf, '\n');/* search last space */
    	*p = '\0';/* split at last space */
	
	strcpy(info, buf);
	
	system("sudo rm -rf temp.txt");
	
}

EApiStatus_t boardinfo_aaeon_init(void)
{
    EApiStatus_t StatusCode = EAPI_STATUS_SUCCESS;
    
    // No initialization required.

    return StatusCode;
}

EApiStatus_t boardinfo_aaeon_deinit(void)
{
    EApiStatus_t StatusCode = EAPI_STATUS_SUCCESS;

    // No deinitialization required.

    return StatusCode;
}

//
//  Get String
//
EApiStatus_t boardinfo_aaeon_get_string_a(__IN uint32_t Id , __OUT char *pBuffer , __INOUT uint32_t *pBufLen) {

	EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
  	uint32_t BufLenSav;
  	char *info = NULL;
  	char path[NAME_MAX] = { 0 };

  	BufLenSav=*pBufLen;
  	info = (char *)calloc(NAME_MAX, sizeof(char));
  	if (!info) {
 	
    		DBG("Error in Allocating Memory");
    		
    		goto fail;
  	}

  	switch(Id) {
    		case EAPI_ID_BOARD_MANUFACTURER_STR: //(0) Board Manufacturer
        		memcpy(path, BOARDINFO_PATH"board_vendor", strlen(BOARDINFO_PATH"board_vendor"));
        	break;
    		case EAPI_ID_BOARD_NAME_STR: //(1) Board Name String
        		memcpy(path, BOARDINFO_PATH"board_name", strlen(BOARDINFO_PATH"board_name"));
        	break;
    		case EAPI_ID_BOARD_REVISION_STR: //(2) Board Name String
        		memcpy(path, BOARDINFO_PATH"board_version", strlen(BOARDINFO_PATH"board_version"));
        	break;
    		case EAPI_ID_BOARD_SERIAL_STR: //(3) Board Serial
        		memcpy(path, BOARDINFO_PATH"board_serial", strlen(BOARDINFO_PATH"board_serial"));
        	break;
    		case EAPI_ID_BOARD_BIOS_REVISION_STR: //(4) Board Bios Revision
        		memcpy(path, BOARDINFO_PATH"bios_version", strlen(BOARDINFO_PATH"bios_version"));
        	break;
    		case EAPI_ID_BOARD_HW_REVISION_STR: //(5) Board Hardware
        		memcpy(path, BOARDINFO_PATH"board_version", strlen(BOARDINFO_PATH"board_version"));
        	break;
    		case EAPI_ID_BOARD_PLATFORM_TYPE_STR: //(6) Platform ID
        		memcpy(info, "Unknown Platform Type", strlen("Unknown Platform Type"));
        	break;
    		case EAPI_ID_PRODUCT_NAME_STR: //(7) Product Name String
        		memcpy(path, BOARDINFO_PATH"product_name", strlen(BOARDINFO_PATH"product_name"));
        	break;
        	case EAPI_ID_BOARD_CPU_MODEL_NAME_STR: //(259) CPU Model Name
        		cpu_open(info);
        		
        	break;	
    		default:
    		
        		DBG("Unrecognised String ID");
        		
        		goto fail;
  	}

  /*
   * Basically equivilant to
   * strncpy(pBuffer, StringLookup[i].StringPtr, *pBufLen);
   * StringLookup[i].StringPtr[*pBufLen-1]='\0';
   * *pBufLen=strlen(StringLookup[i].StringPtr);
   *
   */
   	
   
	if(Id != EAPI_ID_BOARD_CPU_MODEL_NAME_STR) {
		file_open(path, info);
		DBG("EApiBoardGetStringAEmul path =%s, info = %s\n",path, info);
  	} 
 
  	if(info == NULL) {
 		
 		DBG("The board string info is not found.");
    		
    		goto fail;
  	}

	

  	*pBufLen=(uint32_t)(strlen(info)+1); // get info length
  	
  	if(BufLenSav<*pBufLen) StatusCode=EAPI_STATUS_MORE_DATA;
  	else BufLenSav=*pBufLen;

  	if(BufLenSav && (pBuffer!=NULL)) {
    		snprintf(pBuffer,BufLenSav,"%s",info);
    		pBuffer[BufLenSav-1]='\0';
  	}

fail:
	if(info) free(info);
  
  	return StatusCode;
}

//*************//
//  Get Value  //
//*************//
EApiStatus_t boardinfo_aaeon_get_value(__IN uint32_t Id , __OUT uint32_t *pValue) {

	char *object[100];
    	char *type[100];
    	char *ref1[100];
    	char *ref2[100];
    	size_t counter = 0;
	char *line = NULL;
    	size_t length = 0;
    	char lines[NAME_MAX];
    	FILE *input=NULL;
    	char buf[NAME_MAX];
    	char cputemp_cmd[NAME_MAX];
    	char hwm_cmd[NAME_MAX];
    	char hwmtemp_cmd[NAME_MAX];
    	char *p=NULL;
    	int value = 0;
    	long size = 0;
    	int ref1_value = 0;
    	int ref2_value = 0;
    	float fref1_value;
    	FILE *file = fopen(BOARD_CONFIG_PATH, "r");
    	EApiStatus_t StatusCode = EAPI_STATUS_SUCCESS;
    	
    	if(file != NULL) {
    		
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
            			next_token = strtok(NULL, ":");
            			token_category = next_category(token_category);
        		}
        		free(next_token);
        		free(line);
        		++counter;
    		}
    		fclose(file);
	}
	if(Id != EAPI_ID_HWMON_CPU_TEMP) {
		memset(hwmtemp_cmd, 0, sizeof(hwmtemp_cmd));
		strcpy(hwmtemp_cmd, "ls -al ");
		strcat(hwmtemp_cmd, HWM_PATH);
		strcat(hwmtemp_cmd, " | grep ");
		strcat(hwmtemp_cmd, type[3]);
		hwmtemp_cmd[strlen(hwmtemp_cmd) - 1] = '\0';
		strcat(hwmtemp_cmd, " | awk -F '->' '{print $1}' | awk '{print $9}' >> hwmtemp.txt");
		DBG("hwmtemp_cmd is %s\n", hwmtemp_cmd);
		system(hwmtemp_cmd);	
		
		input = fopen("hwmtemp.txt", "r");
		// 使用 fseek 函数将文件指针移动到文件末尾
    		fseek(input, 0, SEEK_END);
    		// 使用 ftell 函数获取文件指针当前位置的偏移量
    		size = ftell(input);
		if(size == 0) {
			fclose(input);
			StatusCode = EAPI_STATUS_ERROR;
			system("rm -rf hwmtemp.txt");
			return StatusCode;
		} else {
			fclose(input);
			input = fopen("hwmtemp.txt", "r");
		}
		memset(buf, 0, NAME_MAX);
		memset(lines, 0, NAME_MAX);
		while (fgets(lines, sizeof(lines), input) != NULL) strcpy( buf, lines);
		p=strrchr(buf, '\n');/* search last space */
		*p = '\0';/* split at last space */
    		memset(hwm_cmd, 0, sizeof(hwm_cmd));
		fclose(input);
		system("sudo rm -rf hwmtemp.txt");
		memcpy(hwm_cmd, "cat ", strlen("cat ")+1);
		strcat(hwm_cmd, HWM_PATH);
		strcat(hwm_cmd, buf);
		strcat(hwm_cmd, "/device/");
		
	}
	switch(Id) {
		case EAPI_ID_HWMON_CPU_TEMP:
			system("ls -al /sys/class/hwmon/ | grep coretemp | awk -F '->' '{print $1}' | awk '{print $9}' >> hwmcputemp.txt");
			input = fopen("hwmcputemp.txt", "r");
			
    			fseek(input, 0, SEEK_END);
    			size = ftell(input);
			if(size == 0) {
				fclose(input);
				StatusCode = EAPI_STATUS_ERROR;
				system("rm -rf hwmcputemp.txt");
				return StatusCode;
			}else {
				fclose(input);
				input = fopen("hwmcputemp.txt", "r");
			}
			
			memset(buf, 0, NAME_MAX);
			memset(lines, 0, NAME_MAX);
			while (fgets(lines, sizeof(lines), input) != NULL) strcpy( buf, lines);
			p=strrchr(buf, '\n');/* search last space */
    			*p = '\0';/* split at last space */
			memset(cputemp_cmd, 0, sizeof(cputemp_cmd));
			fclose(input);
			system("sudo rm -rf hwmcputemp.txt");
			memcpy(cputemp_cmd, "cat ", strlen("cat ")+1);
			strcat(cputemp_cmd, HWM_PATH);
			strcat(cputemp_cmd, buf);
			strcat(cputemp_cmd, "/");
			strcat(cputemp_cmd, type[4]);
			cputemp_cmd[strlen(cputemp_cmd) - 1] = '\0'; //parse last line must be, else not need
			strcat(cputemp_cmd, " >> cputemp.txt");
			system(cputemp_cmd);
			DBG("cputemp_cmd is %s\n",cputemp_cmd);
			input = fopen("cputemp.txt", "r");
			memset(buf, 0, NAME_MAX);
			memset(lines, 0, NAME_MAX);
			while (fgets(lines, sizeof(lines), input) != NULL) strcpy( buf, lines);
			fclose(input);
			system("sudo rm -rf cputemp.txt");
			
    			value = atoi(buf);
			*pValue = (uint32_t ) value;
						
		break;
		case EAPI_ID_HWMON_SYSTEM_TEMP:
			if(strlen(type[5])== 1) {
				StatusCode = EAPI_STATUS_UNSUPPORTED;
				break;
			}
			strcat(hwm_cmd, type[5]);
			hwm_cmd[strlen(hwm_cmd) - 1] = '\0';
			strcat(hwm_cmd, " >> systemp.txt");
			system(hwm_cmd);
			DBG("EAPI_ID_HWMON_SYSTEM_TEMP hwm_cmd is %s\n",hwm_cmd);
			input = fopen("systemp.txt", "r");
			memset(buf, 0, NAME_MAX);
			memset(lines, 0, NAME_MAX);
			while (fgets(lines, sizeof(lines), input) != NULL) strcpy( buf, lines);
			fclose(input);
			system("sudo rm -rf systemp.txt");
			
    			value = atoi(buf);
			*pValue = (uint32_t ) value;
						
		break;
		case EAPI_ID_HWMON_CHIPSET_TEMP:
			if(strlen(type[6])== 1) {
				StatusCode = EAPI_STATUS_UNSUPPORTED;
				break;
			}
			strcat(hwm_cmd, type[6]);
			hwm_cmd[strlen(hwm_cmd) - 1] = '\0';
			strcat(hwm_cmd, " >> chptemp.txt");
			system(hwm_cmd);
			DBG("EAPI_ID_HWMON_CHIPSET_TEMP hwm_cmd is %s\n",hwm_cmd);
			input = fopen("chptemp.txt", "r");
			memset(buf, 0, NAME_MAX);
			memset(lines, 0, NAME_MAX);
			while (fgets(lines, sizeof(lines), input) != NULL) strcpy( buf, lines);
			fclose(input);
			system("sudo rm -rf chptemp.txt");
			
    			value = atoi(buf);
			*pValue = (uint32_t ) value;
			
		break;
		case EAPI_ID_HWMON_VOLTAGE_3V3:
			if(strlen(type[7])== 1) {
				StatusCode = EAPI_STATUS_UNSUPPORTED;
				break;
			}
			strcat(hwm_cmd, type[7]);
			//hwm_cmd[strlen(hwm_cmd) - 1] = '\0';
			strcat(hwm_cmd, " >> 3v3.txt");
			system(hwm_cmd);
			DBG("EAPI_ID_HWMON_VOLTAGE_3V3 hwm_cmd is %s\n",hwm_cmd);
			input = fopen("3v3.txt", "r");
			memset(buf, 0, NAME_MAX);
			memset(lines, 0, NAME_MAX);
			while (fgets(lines, sizeof(lines), input) != NULL) strcpy( buf, lines);
			fclose(input);
			system("sudo rm -rf 3v3.txt");
			
    			value = atoi(buf);
    			ref1_value = atoi(ref1[7]);
    			*pValue = (uint32_t ) (value * ref1_value);
		break;
		case EAPI_ID_HWMON_VOLTAGE_VCORE:
			if(strlen(type[8])== 1) {
				StatusCode = EAPI_STATUS_UNSUPPORTED;
				break;
			}
			strcat(hwm_cmd, type[8]);
			hwm_cmd[strlen(hwm_cmd) - 1] = '\0';
			strcat(hwm_cmd, " >> vcore.txt");
			system(hwm_cmd);
			DBG("EAPI_ID_HWMON_VOLTAGE_VCORE hwm_cmd is %s\n", hwm_cmd);
			input = fopen("vcore.txt", "r");
			memset(buf, 0, NAME_MAX);
			memset(lines, 0, NAME_MAX);
			while (fgets(lines, sizeof(lines), input) != NULL) strcpy( buf, lines);
			fclose(input);
			system("sudo rm -rf vcore.txt");
			
    			value = atoi(buf);
			*pValue = (uint32_t ) value;
						
		break;
		case EAPI_ID_HWMON_VOLTAGE_VBAT:
			if(strlen(type[13])== 1) {
				StatusCode = EAPI_STATUS_UNSUPPORTED;
				break;
			}
			strcat(hwm_cmd, type[13]);
			strcat(hwm_cmd, " >> vbat.txt");
			system(hwm_cmd);
			DBG("EAPI_ID_HWMON_VOLTAGE_VBAT hwm_cmd is %s\n", hwm_cmd);
			input = fopen("vbat.txt", "r");
			memset(buf, 0, NAME_MAX);
			memset(lines, 0, NAME_MAX);
			while (fgets(lines, sizeof(lines), input) != NULL) strcpy( buf, lines);
			fclose(input);
			system("sudo rm -rf vbat.txt");
			
    			value = atoi(buf);
    			ref1_value = atoi(ref1[13]);
			*pValue = (uint32_t ) (value * ref1_value);
						
		break;
		case EAPI_ID_HWMON_VOLTAGE_5V:
			if(strlen(type[10])== 1) {
				StatusCode = EAPI_STATUS_UNSUPPORTED;
				break;
			}
			strcat(hwm_cmd, type[10]);
			strcat(hwm_cmd, " >> 5v.txt");
			system(hwm_cmd);
			DBG("EAPI_ID_HWMON_VOLTAGE_5V hwm_cmd is %s\n", hwm_cmd);
			input = fopen("5v.txt", "r");
			memset(buf, 0, NAME_MAX);
			memset(lines, 0, NAME_MAX);
			while (fgets(lines, sizeof(lines), input) != NULL) strcpy( buf, lines);
			fclose(input);
			system("sudo rm -rf 5v.txt");
			
    			value = atoi(buf);
    			fref1_value = atof(ref1[10]);
    			ref2_value = atoi(ref2[10]);
    			*pValue =(uint32_t ) (((fref1_value + ref2_value) /fref1_value) * value); 
			 							
		break;
		case EAPI_ID_HWMON_VOLTAGE_12V:
			if(strlen(type[11])== 1) {
				StatusCode = EAPI_STATUS_UNSUPPORTED;
				break;
			}
			strcat(hwm_cmd, type[11]);
			strcat(hwm_cmd, " >> 12v.txt");
			system(hwm_cmd);
			DBG("EAPI_ID_HWMON_VOLTAGE_12V hwm_cmd is %s\n", hwm_cmd);
			input = fopen("12v.txt", "r");
			memset(buf, 0, NAME_MAX);
			memset(lines, 0, NAME_MAX);
			while (fgets(lines, sizeof(lines), input) != NULL) strcpy( buf, lines);
			fclose(input);
			system("sudo rm -rf 12v.txt");
			
    			value = atoi(buf);
    			ref1_value = atoi(ref1[11]);
    			ref2_value = atoi(ref2[11]);
			*pValue = (uint32_t ) (((ref1_value + ref2_value) / ref1_value) * value);
						
		break;
		case EAPI_ID_HWMON_VOLTAGE_VMEM:
			if(strlen(type[9])== 1) {
				StatusCode = EAPI_STATUS_UNSUPPORTED;
				break;
			}
			strcat(hwm_cmd, type[9]);
			hwm_cmd[strlen(hwm_cmd) - 1] = '\0';
			strcat(hwm_cmd, " >> vmem.txt");
			system(hwm_cmd);
			DBG("EAPI_ID_HWMON_VOLTAGE_VMEM hwm_cmd is %s\n", hwm_cmd);
			input = fopen("vmem.txt", "r");
			memset(buf, 0, NAME_MAX);
			memset(lines, 0, NAME_MAX);
			while (fgets(lines, sizeof(lines), input) != NULL) strcpy( buf, lines);
			fclose(input);
			system("sudo rm -rf vmem.txt");
			
    			value = atoi(buf);
			*pValue = (uint32_t ) value;
							
		break;
		case EAPI_ID_HWMON_VOLTAGE_3VSB:
			if(strlen(type[12])== 1) {
				StatusCode = EAPI_STATUS_UNSUPPORTED;
				break;
			}
			
			strcat(hwm_cmd, type[12]);
			//hwm_cmd[strlen(hwm_cmd) - 1] = '\0';
			strcat(hwm_cmd, " >> 3vsb.txt");
			system(hwm_cmd);
			DBG("EAPI_ID_HWMON_VOLTAGE_3VSB hwm_cmd is %s\n", hwm_cmd);
			input = fopen("3vsb.txt", "r");
			memset(buf, 0, NAME_MAX);
			memset(lines, 0, NAME_MAX);
			while (fgets(lines, sizeof(lines), input) != NULL) strcpy( buf, lines);
			fclose(input);
			system("sudo rm -rf 3vsb.txt");
			
    			value = atoi(buf);
    			ref1_value = atoi(ref1[12]);
			*pValue = (uint32_t ) (value * ref1_value);
						
		break;
		case EAPI_ID_HWMON_VOLTAGE_5VSB:
			if(strlen(type[14])== 1) {
				StatusCode = EAPI_STATUS_UNSUPPORTED;
				break;
			}
			strcat(hwm_cmd, type[14]);
			//hwm_cmd[strlen(hwm_cmd) - 1] = '\0';
			strcat(hwm_cmd, " >> 5vsb.txt");
			system(hwm_cmd);
			DBG("EAPI_ID_HWMON_VOLTAGE_5VSB hwm_cmd is %s\n", hwm_cmd);
			input = fopen("5vsb.txt", "r");
			memset(buf, 0, NAME_MAX);
			memset(lines, 0, NAME_MAX);
			while (fgets(lines, sizeof(lines), input) != NULL) strcpy( buf, lines);
			fclose(input);
			system("sudo rm -rf 5vsb.txt");
			
    			value = atoi(buf);
    			ref1_value = atoi(ref1[14]);
			*pValue = (uint32_t ) (value * ref1_value);
			
		break;
		case EAPI_ID_HWMON_FAN_CPU:
			if(strlen(type[15])== 1) {
				StatusCode = EAPI_STATUS_UNSUPPORTED;
				break;
			}
			strcat(hwm_cmd, type[15]);
			hwm_cmd[strlen(hwm_cmd) - 1] = '\0';
			strcat(hwm_cmd, " >> cpufan.txt");
			system(hwm_cmd);
			DBG("EAPI_ID_HWMON_FAN_CPU hwm_cmd is %s\n", hwm_cmd);
			input = fopen("cpufan.txt", "r");
			memset(buf, 0, NAME_MAX);
			memset(lines, 0, NAME_MAX);
			while (fgets(lines, sizeof(lines), input) != NULL) strcpy( buf, lines);
			fclose(input);
			system("sudo rm -rf cpufan.txt");
			
    			value = atoi(buf);
			*pValue = (uint32_t ) value;
			
		break;
		default:
			StatusCode = EAPI_STATUS_UNSUPPORTED;
        	break;		
	}
	
	DBG("Hardware Monitor Id = 0x%5X, Value = %d\n",Id, *pValue);

	return StatusCode;
}



