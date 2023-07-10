
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/sysinfo.h>
#include <limits.h> 

#include <hook_backlight.h>
#include <hook_boardinfo.h>

static unsigned int BacklightFound = 0;
static char backlight_brightness_path[NAME_MAX] = "";
static char backlight_Enable_path[NAME_MAX] = "";
static char backlight1_brightness_path[NAME_MAX] = "";
static char uinit_backlight1_path[NAME_MAX] = "";

EApiStatus_t blctrl_init(void) {

	EApiStatus_t StatusCode = EAPI_STATUS_NOT_INITIALIZED;
    
    	FILE *file = fopen(BOARD_CONFIG_PATH, "r");
    	if(file != NULL) {
    		char *object[100];
    		char *type[100];
    		char *ref1[100];
    		char *ref2[100];
    		size_t counter = 0;
		char *line = NULL;
    		size_t length = 0;
    		char backlight_check[NAME_MAX];
    		char backlight_cmd[NAME_MAX] = "echo ad5247 0x2e > ";
    		char lines[NAME_MAX];
    		char buf[NAME_MAX];
    		long size = 0;
    		char *p=NULL;
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
    		
    		memset(backlight_check, 0, sizeof(backlight_check));
		strcpy(backlight_check, "lsmod | grep ");
		strcat(backlight_check, type[19]);
		backlight_check[strlen(backlight_check) - 1] = '\0';
		strcat(backlight_check, " >> backlightemp.txt");
		DBG("backlight_check is %s\n", backlight_check);
		system(backlight_check);	
		
		file = fopen("backlightemp.txt", "r");
		fseek(file, 0, SEEK_END);
    		size = ftell(file);
    		fclose(file);
		system("rm -rf backlightemp.txt");
		if(size == 0) return StatusCode;
		    		    		
    		memset(backlight_check, 0, sizeof(backlight_check));
    		
    		strcpy(backlight_check, "ls -al ");
		strcat(backlight_check, BACKLIGHT_PATH);
		strcat(backlight_check, " | grep ");
		strcat(backlight_check, type[19]);
		backlight_check[strlen(backlight_check) - 1] = '\0';
		strcat(backlight_check, " | awk -F '->' '{print $1}' | awk '{print $9}' >> backlightemp.txt");
		DBG("backlight_check is %s\n", backlight_check);
		
		system(backlight_check);
		
		file = fopen("backlightemp.txt", "r");
		memset(buf, 0, NAME_MAX);
		memset(lines, 0, NAME_MAX);
		while (fgets(lines, sizeof(lines), file) != NULL) strcpy( buf, lines);
		p=strrchr(buf, '\n');/* search last space */
		*p = '\0';/* split at last space */
		
		fclose(file);
		system("rm -rf backlightemp.txt");
		
    		strcpy(backlight_brightness_path, BACKLIGHT_PATH);
    		strcpy(backlight_Enable_path, BACKLIGHT_PATH);
    		
    		strcat(backlight_brightness_path, buf);
    		strcat(backlight_Enable_path, buf);
    		    		
    		strcat(backlight_brightness_path, "/");
    		strcat(backlight_Enable_path, "/");
    		    		
    		strcat(backlight_brightness_path, type[21]);
    		backlight_brightness_path[strlen(backlight_brightness_path) - 1] = '\0';
    		strcat(backlight_Enable_path, type[20]);
    		backlight_Enable_path[strlen(backlight_Enable_path) - 1] = '\0';
    		
    		DBG("backlight_brightness_path is %s\n", backlight_brightness_path);
    		DBG("backlight_Enable_path is %s\n", backlight_Enable_path);
    		
    		memset(backlight_check, 0, sizeof(backlight_check));
    		
    		strcpy(backlight_check, "ls -al ");
		strcat(backlight_check, BACKLIGHT_PATH);
		strcat(backlight_check, " | grep ");
		strcat(backlight_check, type[19]);
		backlight_check[strlen(backlight_check) - 1] = '\0';
		strcat(backlight_check, " | awk -F '->' '{print $2}' | awk -F '/' '{print $6}' >> backlightemp.txt");
		DBG("backlight_check is %s\n", backlight_check);
    		
    		system(backlight_check);
		
		file = fopen("backlightemp.txt", "r");
		memset(buf, 0, NAME_MAX);
		memset(lines, 0, NAME_MAX);
		while (fgets(lines, sizeof(lines), file) != NULL) strcpy( buf, lines);
		p=strrchr(buf, '\n');/* search last space */
		*p = '\0';/* split at last space */
		
		fclose(file);
		system("rm -rf backlightemp.txt");
		
    		strcpy(backlight1_brightness_path, AD5247_PATH);
    		strcat(backlight1_brightness_path, buf);
    		strcat(backlight_cmd, backlight1_brightness_path);
    		strcat(backlight_cmd, "/new_device");
    		DBG("backlight_cmd is %s\n", backlight_cmd);
    		system(backlight_cmd);
    		strcpy(uinit_backlight1_path, backlight1_brightness_path);
    		strcat(backlight1_brightness_path, "/0-002e/rdac0");
    		strcat(uinit_backlight1_path, "/delete_device");
    		DBG("backlight1_brightness_path is %s\n", backlight1_brightness_path);
    		    		
    		BacklightFound = 1;    		    		
    		StatusCode = EAPI_STATUS_SUCCESS;
 
     	}	
   	
    	return StatusCode;
}

EApiStatus_t blctrl_deinit(void) {

    EApiStatus_t StatusCode = EAPI_STATUS_SUCCESS;
    char backlight_cmd[NAME_MAX] = "echo 0x2e > ";
    strcat(backlight_cmd, uinit_backlight1_path);
    system(backlight_cmd);

    return StatusCode;
}

EApiStatus_t blctrl_get_enable(__IN uint32_t Id __attribute__ ((unused)), __OUT uint32_t *pEnable __attribute__ ((unused))) {

	EApiStatus_t StatusCode = EAPI_STATUS_NOT_INITIALIZED;
	
	DBG("%s: Id=%d\n", __func__, Id);
    	
    	if(BacklightFound == 0) return StatusCode;
    	
    	if(Id == EAPI_ID_BACKLIGHT_2) {
    		StatusCode = EAPI_STATUS_UNSUPPORTED;
    		return StatusCode;
    	}
    	
    	char backlight_Enable_cmd[NAME_MAX] = "";
    	FILE *file = NULL;
    	char lines[NAME_MAX];
    	char buf[NAME_MAX];
    	char *p=NULL;
    	uint32_t value;
    	
    	strcpy(backlight_Enable_cmd, "cat ");
    	strcat(backlight_Enable_cmd, backlight_Enable_path);
    	strcat(backlight_Enable_cmd, " >> backlightemp.txt");
    	DBG("backlight_Enable_cmd is %s\n", backlight_Enable_cmd);
    	system(backlight_Enable_cmd);
    	
    	file = fopen("backlightemp.txt", "r");
	memset(buf, 0, NAME_MAX);
	memset(lines, 0, NAME_MAX);
	while (fgets(lines, sizeof(lines), file) != NULL) strcpy( buf, lines);
	p=strrchr(buf, '\n');/* search last space */
	*p = '\0';/* split at last space */
		
	fclose(file);
	system("rm -rf backlightemp.txt");
    	value = (uint32_t)(atoi(buf));
    	DBG("value is %u\n", value);
    	if(value != 0) *pEnable = EAPI_BACKLIGHT_SET_OFF;  
    	else *pEnable = EAPI_BACKLIGHT_SET_ON;
    	
	StatusCode = EAPI_STATUS_SUCCESS;
    	return StatusCode;
}

EApiStatus_t blctrl_set_enable(__IN uint32_t Id __attribute__ ((unused)), __IN uint32_t Enable __attribute__ ((unused))) {

	EApiStatus_t StatusCode = EAPI_STATUS_NOT_INITIALIZED;

    	DBG("%s: Id=%d\n", __func__, Id);
    	
    	if(BacklightFound == 0) return StatusCode;
    	
    	if(Id == EAPI_ID_BACKLIGHT_2) {
    		StatusCode = EAPI_STATUS_UNSUPPORTED;
    		return StatusCode;
    	}
    	
    	char backlight_Enable_cmd[NAME_MAX] = "";
    	
    	if(Enable == EAPI_BACKLIGHT_SET_ON) strcpy(backlight_Enable_cmd, "echo 0 > ");
    	else strcpy(backlight_Enable_cmd, "echo 1 > ");
    	
    	strcat(backlight_Enable_cmd, backlight_Enable_path);
    	DBG("backlight_Enable_cmd is %s\n", backlight_Enable_cmd);
    	system(backlight_Enable_cmd);

	StatusCode = EAPI_STATUS_SUCCESS;
    	return StatusCode;
    
}

EApiStatus_t blctrl_get_brightness(__IN uint32_t Id, __OUT uint32_t *pBrightness) {

	EApiStatus_t StatusCode = EAPI_STATUS_NOT_INITIALIZED;

    	DBG("%s: Id=%d\n", __func__, Id);
    
    	if(BacklightFound == 0) return StatusCode;
    	
    	char backlight_brightness_cmd[NAME_MAX] = "";
    	FILE *file = NULL;
    	char lines[NAME_MAX];
    	char buf[NAME_MAX];
    	char *p=NULL;
    	    	
    	strcpy(backlight_brightness_cmd, "cat ");
    	
    	switch(Id) {
    		case EAPI_ID_BACKLIGHT_1:
    			strcat(backlight_brightness_cmd, backlight_brightness_path);
    		break;
    		case EAPI_ID_BACKLIGHT_2:
    			strcat(backlight_brightness_cmd, backlight1_brightness_path);
    		break;
    	
    	}
    	
    	strcat(backlight_brightness_cmd, " >> backlightemp.txt");
    	DBG("backlight_brightness_cmd is %s\n", backlight_brightness_cmd);
	
	system(backlight_brightness_cmd);
    	
    	file = fopen("backlightemp.txt", "r");
	memset(buf, 0, NAME_MAX);
	memset(lines, 0, NAME_MAX);
	while (fgets(lines, sizeof(lines), file) != NULL) strcpy( buf, lines);
	p=strrchr(buf, '\n');/* search last space */
	*p = '\0';/* split at last space */
		
	fclose(file);
	system("rm -rf backlightemp.txt");
    	*pBrightness = (uint32_t)(atoi(buf));
    	DBG("pBrightness is %u\n", *pBrightness);
    	
	StatusCode = EAPI_STATUS_SUCCESS;
    	return StatusCode;
}

EApiStatus_t blctrl_set_brightness(__IN uint32_t Id, __IN uint32_t Brightness) {

	EApiStatus_t StatusCode = EAPI_STATUS_NOT_INITIALIZED;

    	DBG("%s: Id=%d\n", __func__, Id);
    
    	if(BacklightFound == 0) return StatusCode;
    	
    	char backlight_brightness_cmd[NAME_MAX] = "";
    	
    	sprintf(backlight_brightness_cmd, "echo %u > ", Brightness);
    	    	
    	switch(Id) {
    		case EAPI_ID_BACKLIGHT_1:
    			strcat(backlight_brightness_cmd, backlight_brightness_path);
    		break;
    		case EAPI_ID_BACKLIGHT_2:
    			strcat(backlight_brightness_cmd, backlight1_brightness_path);		
    		break;
    	}
    	
    	DBG("backlight_brightness_cmd is %s\n", backlight_brightness_cmd);    	
    	system(backlight_brightness_cmd);
    	
    	StatusCode = EAPI_STATUS_SUCCESS;
    	return StatusCode;
}
