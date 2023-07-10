#include <EApiLib.h>
#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/watchdog.h>

#include <hook_boardinfo.h>

#define WATCHDOGDEV "/dev/watchdog"

static unsigned int WatchdogFound = 0;
static int wdogDescriptor = 0;
static uint32_t MaxDelayValue = 0;
static uint32_t MaxResetTimeoutValue = 0;

EApiStatus_t watchdog_aaeon_init(void) {

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
    		char wdt_check[100];
    		char wdt_maxtimecmd[100];
    		char wdt_mintimecmd[100];
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
    		
    		memset(wdt_check, 0, sizeof(wdt_check));
		strcpy(wdt_check, "lsmod | grep ");
		strcat(wdt_check, type[16]);
		wdt_check[strlen(wdt_check) - 1] = '\0';
		strcat(wdt_check, " >> wdttemp.txt");
		DBG("wdt_check is %s\n", wdt_check);
		system(wdt_check);	
		
		file = fopen("wdttemp.txt", "r");
		// 使用 fseek 函数将文件指针移动到文件末尾
    		fseek(file, 0, SEEK_END);
    		// 使用 ftell 函数获取文件指针当前位置的偏移量
    		size = ftell(file);
    		fclose(file);
		system("rm -rf wdttemp.txt");
		if(size == 0) return StatusCode;
		
		wdogDescriptor = open(WATCHDOGDEV, O_WRONLY);
    		if (wdogDescriptor == -1) {
        		DBG("Can't open watchdog device/n");
        		return StatusCode;
        	}
        	
        	//set default watchdog disable
		int option = WDIOS_DISABLECARD ;
		ioctl(wdogDescriptor, WDIOC_SETOPTIONS, &option);   
		    
		WatchdogFound = 1;    		    		
    		StatusCode = EAPI_STATUS_SUCCESS;
    		
    		memset(wdt_maxtimecmd, 0, sizeof(wdt_maxtimecmd));
    		memset(wdt_mintimecmd, 0, sizeof(wdt_mintimecmd));
    		memset(wdt_check, 0, sizeof(wdt_check));
    		
    		strcpy(wdt_check, "ls -al ");
		strcat(wdt_check, WDT_PATH);
		strcat(wdt_check, " | grep ");
		strcat(wdt_check, type[16]);
		wdt_check[strlen(wdt_check) - 1] = '\0';
		strcat(wdt_check, " | awk -F '->' '{print $1}' | awk '{print $9}' >> wdttemp.txt");
		DBG("wdt_check is %s\n", wdt_check);
		system(wdt_check);
		
		file = fopen("wdttemp.txt", "r");
		memset(buf, 0, NAME_MAX);
		memset(lines, 0, NAME_MAX);
		while (fgets(lines, sizeof(lines), file) != NULL) strcpy( buf, lines);
		p=strrchr(buf, '\n');/* search last space */
		*p = '\0';/* split at last space */
		
		fclose(file);
		system("rm -rf wdttemp.txt");
    		
    		strcpy(wdt_maxtimecmd, "cat ");
    		strcpy(wdt_mintimecmd, "cat ");
    		strcat(wdt_maxtimecmd, WDT_PATH);
    		strcat(wdt_mintimecmd, WDT_PATH);
    		strcat(wdt_maxtimecmd, buf);
    		strcat(wdt_mintimecmd, buf);
    		strcat(wdt_maxtimecmd, "/");
    		strcat(wdt_mintimecmd, "/");
    		strcat(wdt_maxtimecmd, type[17]);
    		wdt_maxtimecmd[strlen(wdt_maxtimecmd) - 1] = '\0';
    		strcat(wdt_mintimecmd, type[18]);
    		wdt_mintimecmd[strlen(wdt_mintimecmd) - 1] = '\0';
    		strcat(wdt_maxtimecmd, " >> maxtime.txt");
    		strcat(wdt_mintimecmd, " >> mintime.txt");    		
    		    		    		
    		DBG("wdt_maxtimecmd is %s\n", wdt_maxtimecmd);
    		DBG("wdt_mintimecmd is %s\n", wdt_mintimecmd);
    		system(wdt_maxtimecmd);
    		system(wdt_mintimecmd);
    		
    		file = fopen("maxtime.txt", "r");
		memset(buf, 0, NAME_MAX);
		memset(lines, 0, NAME_MAX);
		while (fgets(lines, sizeof(lines), file) != NULL) strcpy( buf, lines);
		p=strrchr(buf, '\n');/* search last space */
		*p = '\0';/* split at last space */
		
		fclose(file);
		system("rm -rf maxtime.txt");
    		MaxResetTimeoutValue = (uint32_t)(atoi(buf));
    		
    		file = fopen("mintime.txt", "r");
		memset(buf, 0, NAME_MAX);
		memset(lines, 0, NAME_MAX);
		while (fgets(lines, sizeof(lines), file) != NULL) strcpy( buf, lines);
		p=strrchr(buf, '\n');/* search last space */
		*p = '\0';/* split at last space */
		
		fclose(file);
		system("rm -rf mintime.txt");
    		
    		MaxDelayValue = (uint32_t)(atoi(buf));
    		
    		DBG("MaxResetTimeoutValue is %u, MaxDelayValue is %u\n", MaxResetTimeoutValue, MaxDelayValue);
    		
    	}	
    	
    	return StatusCode;
}

EApiStatus_t watchdog_aaeon_deinit(void) {

	EApiStatus_t StatusCode = EAPI_STATUS_SUCCESS;

    	close(wdogDescriptor);
    	
    	WatchdogFound = 0;

    	return StatusCode;
}

EApiStatus_t watchdog_aaeon_get_cap(__OUTOPT uint32_t *pMaxDelay , __OUTOPT uint32_t *pMaxEventTimeout, __OUTOPT uint32_t *pMaxResetTimeout) {

	EApiStatus_t StatusCode = EAPI_STATUS_NOT_INITIALIZED;

	DBG("wdog %s\n", __func__);

  	if (WatchdogFound == 0) {
    		DBG("WatchDog not found\n");
    		*pMaxDelay = 0;
    		*pMaxEventTimeout = 0 ;
    		*pMaxResetTimeout = 0;
    		return StatusCode;
  	}
  	  	 	
  	
  	DBG("WatchDog found\n");
  	DBG("%s MaxDelay=%u sec, MaxResetTimeout=%u sec\n", __func__,  MaxDelayValue, MaxResetTimeoutValue);
  	
  	*pMaxDelay = (MaxDelayValue * 1000); /* in msec */
  	*pMaxEventTimeout = 0; //because not support
  	*pMaxResetTimeout = (MaxResetTimeoutValue * 1000); /* in msec */
  	
	StatusCode=EAPI_STATUS_SUCCESS;
  	return StatusCode;
}

EApiStatus_t watchdog_aaeon_start(__IN uint32_t Delay , __IN uint32_t EventTimeout __attribute__ ((unused)), __IN uint32_t ResetTimeout) {

	EApiStatus_t StatusCode=EAPI_STATUS_NOT_INITIALIZED;

  	DBG("wdog %s Delay=%d, EventTimeout=%d, ResetTimeout=%d msec \n", __func__, Delay, EventTimeout, ResetTimeout);
  	
  	if (WatchdogFound == 0) return StatusCode;

  	if ((EventTimeout > 0) || (Delay < 1000)) {
    		StatusCode= EAPI_STATUS_UNSUPPORTED;
    		return StatusCode;
  	}
  	
  	if (((MaxDelayValue * 1000) < Delay) || (ResetTimeout > (MaxResetTimeoutValue * 1000))) {
  		StatusCode = EAPI_STATUS_INVALID_PARAMETER;
  		return StatusCode;
  	}

      	uint32_t ResetTimeoutSec = ResetTimeout / 1000;
    	
    	if (ioctl(wdogDescriptor, WDIOC_SETTIMEOUT, &ResetTimeoutSec) != 0) {
        	DBG("Can't set watchdog timeout/n");
        	StatusCode= EAPI_STATUS_UNSUPPORTED;
    		return StatusCode;
        }
        
        int option = WDIOS_ENABLECARD ;
	ioctl(wdogDescriptor, WDIOC_SETOPTIONS, &option);
  
	StatusCode = EAPI_STATUS_SUCCESS;
  
  	return StatusCode;
}

EApiStatus_t watchdog_aaeon_trigger(void) {

	EApiStatus_t StatusCode=EAPI_STATUS_NOT_INITIALIZED;
	DBG("wdog %s\n", __func__);
  	  	
  	if (WatchdogFound == 0) return StatusCode;
  	//keep live
  	ioctl(wdogDescriptor, WDIOC_KEEPALIVE, 0);
  	StatusCode = EAPI_STATUS_SUCCESS;
	
  	return StatusCode;
}

EApiStatus_t watchdog_stop(void) {

	EApiStatus_t StatusCode=EAPI_STATUS_NOT_INITIALIZED;
	DBG("wdog %s\n", __func__);
  	  	
  	if (WatchdogFound == 0) return StatusCode;
	int option = WDIOS_DISABLECARD ;
	ioctl(wdogDescriptor, WDIOC_SETOPTIONS, &option); 
	
  	StatusCode = EAPI_STATUS_SUCCESS;
	
	return StatusCode;
}



