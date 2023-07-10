#include <fcntl.h>
#include <hook_gpio.h>
#include <hook_boardinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <DbgChk.h>

// GPIO mapping between EAPI GPIO and Hardware(Device) GPIO
#define IS_BANK_ID(eapi_id)     ((0x10000 & eapi_id) > 0)

// 32 pins EAPI GPIO per EAPI BANK, refer to EApi.h
// bank0: gpio={0..31}
// bank1: gpio={32..63}
// bank2: gpio={64..95}
#define EAPI_GPIO_NUMBER_PER_SET                0x20 // 32
#define EAPI_GPIO_BANK_SET(eapi_id)             (0xFFFF & eapi_id)
#define EAPI_GPIO_BANK_GPIO_ID_BASE(eapi_id)    (EAPI_GPIO_BANK_SET(eapi_id) * EAPI_GPIO_NUMBER_PER_SET)

// Bit Operation
#define BIT_VALUE(bitmask, bit) ((bitmask >> bit) & 0x1)


int gpio_count = 0;
int gpio_pinnum = 0;


EApiStatus_t gpio_init(void) {

	EApiStatus_t StatusCode = EAPI_STATUS_UNSUPPORTED;
    
    	FILE *file = fopen(BOARD_CONFIG_PATH, "r");
    	if(file != NULL) {
    		char *object[100];
    		char *type[100];
    		char *ref1[100];
    		char *ref2[100];
    		size_t counter = 0;
		char *line = NULL;
    		size_t length = 0;
    		char export_cmd[100];
    		char gpio_check[100];
    		int num=0;
    		long size = 0;
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
    		
    		memset(gpio_check, 0, sizeof(gpio_check));
		strcpy(gpio_check, "lsmod | grep ");
		strcat(gpio_check, type[2]);
		gpio_check[strlen(gpio_check) - 1] = '\0';
		strcat(gpio_check, " >> gpiotemp.txt");
		DBG("gpio_check is %s\n", gpio_check);
		system(gpio_check);	
		
		file = fopen("gpiotemp.txt", "r");
		// 使用 fseek 函数将文件指针移动到文件末尾
    		fseek(file, 0, SEEK_END);
    		// 使用 ftell 函数获取文件指针当前位置的偏移量
    		size = ftell(file);
    		fclose(file);
		system("rm -rf gpiotemp.txt");
		if(size == 0) {
			StatusCode = EAPI_STATUS_UNSUPPORTED;
			return StatusCode;
		}
    		gpio_count = atoi(type[0]);
    		gpio_pinnum = atoi(type[1]);
    		
    		DBGInfo("gpio_count: %d,  gpio_pinnum: %d\n", gpio_count, gpio_pinnum);
    		for(int i=0; i < gpio_count; i++) {
    			memset(export_cmd, 0, sizeof(export_cmd));
    			num = gpio_pinnum + i;
    			sprintf(export_cmd, "echo %d > ", num);
    			strcat(export_cmd, GPIO_PATH);
    			strcat(export_cmd, "export");
    			DBG("%s\n", export_cmd);	
    			system(export_cmd);     		
    		}
    		    		
    		StatusCode = EAPI_STATUS_SUCCESS;
    	}	
    	
    	return StatusCode;
}

EApiStatus_t gpio_deinit(void) {

	EApiStatus_t StatusCode = EAPI_STATUS_UNSUPPORTED;
	
	if(gpio_count != 0) {
		char unexport_cmd[100];
		int num=0;
		for(int i=0; i < gpio_count; i++) {
    			memset(unexport_cmd, 0, sizeof(unexport_cmd));
    			num = gpio_pinnum + i;
    			sprintf(unexport_cmd, "echo %d > ", num);
    			strcat(unexport_cmd, GPIO_PATH);
    			strcat(unexport_cmd, "unexport");
    			DBG("%s\n", unexport_cmd);	
    			system(unexport_cmd);     		
    		}
    		    		
    		StatusCode = EAPI_STATUS_SUCCESS;
	}
	
    	return StatusCode;
}

static uint32_t single_gpio_get_lv(uint32_t gpio_id) {

	uint32_t level = 0;
	char getlv_cmd[100];
	char gpioname[10];
	char lines[256];
    	FILE *input;
    	char buf[10];
    	int num=0;
    	num =  gpio_pinnum + gpio_id;
    	memset(getlv_cmd, 0, sizeof(getlv_cmd));
    	memset(gpioname, 0, sizeof(gpioname));
    	sprintf(gpioname, "gpio%d/", num);
    	memcpy(getlv_cmd, "cat ", strlen("cat ")+1);
    	strcat(getlv_cmd, GPIO_PATH);
    	strcat(getlv_cmd, gpioname);
    	strcat(getlv_cmd, "value >> gpiovalue");
    	
    	system(getlv_cmd);
    	input = fopen("gpiovalue", "r");
	while (fgets(lines, sizeof(lines), input) != NULL) strcpy( buf, lines);
			
	fclose(input);
	level = atoi(buf);
    	system("rm -rf gpiovalue");
    	DBG("Output level=%d\n", level);
    	return level;
}

static EApiStatus_t bank_gpio_get_lv(__IN EApiId_t Id, __IN uint32_t BitMask, __OUT uint32_t *pLevel)
{
    EApiStatus_t StatusCode = EAPI_STATUS_UNSUPPORTED;
    uint32_t idx;
    uint32_t level = 0; // bit values for all GPIO pins of a bank
    uint32_t eapi_gpio_id;

    for (eapi_gpio_id = EAPI_GPIO_BANK_GPIO_ID_BASE(Id), idx = 0;
         idx < EAPI_GPIO_NUMBER_PER_SET;
         eapi_gpio_id++, idx++)
    {
        if (BIT_VALUE(BitMask, idx) == 0x1) {
            level = single_gpio_get_lv(eapi_gpio_id);
            if ((level != EAPI_GPIO_LOW) && (level != EAPI_GPIO_HIGH)) {
                // invalid value, it should be a return code
                StatusCode = level;
                goto failure;
            }
            *pLevel |= (level << idx);
        }
    }

    StatusCode = EAPI_STATUS_SUCCESS;

failure:
    DBG("StatusCode = 0x%x\n", StatusCode);

    return StatusCode;
}

EApiStatus_t gpio_get_lv(__IN EApiId_t Id, __IN uint32_t BitMask, __OUT uint32_t *pLevel)
{
    EApiStatus_t StatusCode = EAPI_STATUS_UNSUPPORTED;
    uint32_t gpio_id;

    DBG("Id:%d, BitMask=0x%x\n", Id, BitMask);

    if (IS_BANK_ID(Id)) {
        *pLevel = 0;
        if (!EAPI_TEST_SUCCESS(bank_gpio_get_lv(Id, BitMask, pLevel))) {
            goto failure;
        }
        DBG("Bank Level=0x%x\n", *pLevel);
    } else {
        // Don't care BitMask, the bit value of single port addressing should be always 1.
        // It is no meaning if the bit value is 0.
        gpio_id = Id;
        *pLevel = single_gpio_get_lv(gpio_id);
        if ((*pLevel != EAPI_GPIO_LOW) && (*pLevel != EAPI_GPIO_HIGH)) {
            // invalid Level value, it should be a return code
            StatusCode = *pLevel;
            goto failure;
        }
        DBG("Single Level=0x%x\n", *pLevel);
    }

    StatusCode = EAPI_STATUS_SUCCESS;

failure:
    DBG("StatusCode = 0x%x\n", StatusCode);
    return StatusCode;
}

static EApiStatus_t single_gpio_set_lv(uint32_t gpio_id, uint32_t level) {

	EApiStatus_t StatusCode = EAPI_STATUS_UNSUPPORTED;
	
	if(level > 1) goto set_lv_err;
    
    	char setlv_cmd[100];
	char gpioname[10];
	int num=0;
    	num =  gpio_pinnum + gpio_id;
	memset(setlv_cmd, 0, sizeof(setlv_cmd));
    	memset(gpioname, 0, sizeof(gpioname));
    	sprintf(gpioname, "gpio%d/", num);
    	sprintf(setlv_cmd, "echo %d >", level);
    	
    	strcat(setlv_cmd, GPIO_PATH);
    	strcat(setlv_cmd, gpioname);
    	strcat(setlv_cmd, "value");
    	
    	system(setlv_cmd);
    	
    	StatusCode = EAPI_STATUS_SUCCESS;
    
set_lv_err:    
    	DBG("StatusCode = 0x%x\n", StatusCode);

    	return StatusCode;
}

static EApiStatus_t bank_gpio_set_lv(__IN EApiId_t Id, __IN uint32_t BitMask __attribute__ ((unused)), uint32_t Level)
{
    EApiStatus_t StatusCode = EAPI_STATUS_UNSUPPORTED;
    uint32_t idx;
    uint32_t eapi_gpio_id, gpio_id;

    DBG("bank_gpio_set_lv Id:%d, BitMask=0x%x, Level=%x\n", Id, BitMask, Level);
    eapi_gpio_id = EAPI_GPIO_BANK_GPIO_ID_BASE(Id);
    DBG("bank_gpio_set_lv eapi_gpio_id = %x\n", eapi_gpio_id);
    for (eapi_gpio_id = EAPI_GPIO_BANK_GPIO_ID_BASE(Id), idx = 0;
         idx < EAPI_GPIO_NUMBER_PER_SET;
         eapi_gpio_id++, idx++)
    {
        gpio_id = eapi_gpio_id;
        if ((BitMask & (1 << idx)) && (gpio_id < 0x8)) {
            StatusCode = single_gpio_set_lv(gpio_id, (uint32_t)((Level & (0x1 << idx)) >> idx));
        }
    }

    return StatusCode;
}

EApiStatus_t gpio_set_lv(__IN EApiId_t Id, __IN uint32_t BitMask, __IN uint32_t Level)
{
    EApiStatus_t StatusCode = EAPI_STATUS_UNSUPPORTED;
    uint32_t gpio_id;

    DBG("Id:%d, BitMask=0x%x, Level=%x\n", Id, BitMask, Level);

    if (IS_BANK_ID(Id)) {
        DBG("gpio_set_lv bank_gpio_set_lv\n");
        StatusCode = bank_gpio_set_lv(Id, BitMask, Level);
    } else {
        gpio_id = Id;
        DBG("gpio_set_lv single_gpio_set_lv\n");
        StatusCode = single_gpio_set_lv(gpio_id, Level);
    }

    DBG("StatusCode = 0x%x\n", StatusCode);
    return StatusCode;
}

uint32_t single_gpio_get_dir(uint32_t gpio_id) {

	uint32_t direction = 0; 
	char getdir_cmd[100];
	char gpioname[10];
	FILE *input;
    	char buf[4];
    	char dir[4] = "in";
    	int i=0, num=0;
    	char c;
    	num =  gpio_pinnum + gpio_id;
    	memset(getdir_cmd, 0, sizeof(getdir_cmd));
    	memset(gpioname, 0, sizeof(gpioname));
    	sprintf(gpioname, "gpio%d/", num);
    	memcpy(getdir_cmd, "cat ", strlen("cat ")+1);
    	strcat(getdir_cmd, GPIO_PATH);
    	strcat(getdir_cmd, gpioname);
    	strcat(getdir_cmd, "direction >> gpiodir");
    	
    	system(getdir_cmd);
    	input = fopen("gpiodir", "r");
	while((c = fgetc(input)) != EOF) {
		buf[i] = c;
		i++;
	}
	fclose(input);
	buf[i] = '\0';
	buf[strlen(buf) - 1] = '\0';
	if(strcmp(dir, buf) == 0) direction = 1;
	else direction = 0;
    	system("rm -rf gpiodir");
    	
    	DBG("single_gpio_get_dir Output direction = %d\n", direction);
    	return direction;
}

static EApiStatus_t bank_gpio_get_dir(__IN EApiId_t Id, __IN uint32_t BitMask, uint32_t *pDirection)
{
    EApiStatus_t StatusCode = EAPI_STATUS_UNSUPPORTED;
    uint32_t idx;
    uint32_t direction = 0; // bit values for all GPIO pins of a bank
    uint32_t eapi_gpio_id;
    DBG("bank_gpio_get_dir Id:%d, pDirection: %d\n", Id, *pDirection);

    for (eapi_gpio_id = EAPI_GPIO_BANK_GPIO_ID_BASE(Id), idx = 0;
         idx < EAPI_GPIO_NUMBER_PER_SET;
         eapi_gpio_id++, idx++)
    {
        if (BIT_VALUE(BitMask, idx) == 0x1) {
            direction = single_gpio_get_dir(eapi_gpio_id);
            if ((direction != EAPI_GPIO_INPUT) && (direction != EAPI_GPIO_OUTPUT)) {
                // invalid value, it should be a return code
                StatusCode = direction;
                goto failure;
            }
            *pDirection |= (direction << idx);
        }
    }

    StatusCode = EAPI_STATUS_SUCCESS;

failure:
    DBG("StatusCode = 0x%x\n", StatusCode);
    return StatusCode;
}

EApiStatus_t gpio_get_dir(__IN EApiId_t Id, __IN uint32_t BitMask, __OUT uint32_t *pDirection)
{
    EApiStatus_t StatusCode = EAPI_STATUS_UNSUPPORTED;
    uint32_t gpio_id;

    DBG("gpio_get_dir Id:%d, BitMask=0x%x\n", Id, BitMask);

    if (IS_BANK_ID(Id)) {
    	*pDirection = 0;
        if (!EAPI_TEST_SUCCESS(bank_gpio_get_dir(Id, BitMask, pDirection))) {
            goto failure;
        }
        DBG("Bank Direction=0x%x\n", *pDirection);
    } else {
        gpio_id = Id;
        if(gpio_id < 8){
            *pDirection = single_gpio_get_dir(gpio_id);
            if ((*pDirection != EAPI_GPIO_INPUT) && (*pDirection != EAPI_GPIO_OUTPUT)) {
                // invalid value, it should be a return code
                StatusCode = *pDirection;
                goto failure;
            }
            DBG("Single Direction=0x%x\n", *pDirection);
        }
    }

    StatusCode = EAPI_STATUS_SUCCESS;

failure:
    DBG("StatusCode = 0x%x\n", StatusCode);
    return StatusCode;
}

static EApiStatus_t single_gpio_set_dir(uint32_t gpio_id, uint32_t direction)
{
    EApiStatus_t StatusCode = EAPI_STATUS_UNSUPPORTED;
    if(direction > 1) goto set_dir_err;
    
    	char setdir_cmd[100];
	char gpioname[10];
	int num=0;
	
	num =  gpio_pinnum + gpio_id;
	memset(setdir_cmd, 0, sizeof(setdir_cmd));
    	memset(gpioname, 0, sizeof(gpioname));
    	sprintf(gpioname, "gpio%d/", num);
    	if(direction == 0) memcpy(setdir_cmd, "echo \"out\" >", strlen("echo \"out\" >"));
    	else memcpy(setdir_cmd, "echo \"in\" >", strlen("echo \"in\" >"));
    	strcat(setdir_cmd, GPIO_PATH);
    	strcat(setdir_cmd, gpioname);
    	strcat(setdir_cmd, "direction");
    	DBG("%s\n", setdir_cmd);
    	system(setdir_cmd);
    	
    	StatusCode = EAPI_STATUS_SUCCESS;
    
set_dir_err:    
    	DBG("StatusCode = 0x%x\n", StatusCode);

    	return StatusCode;
    
}

static EApiStatus_t bank_gpio_set_dir(__IN EApiId_t Id, __IN uint32_t BitMask, __IN uint32_t Direction)
{
    EApiStatus_t StatusCode = EAPI_STATUS_UNSUPPORTED;
    uint32_t idx;
    uint32_t eapi_gpio_id, gpio_id;

    eapi_gpio_id = EAPI_GPIO_BANK_GPIO_ID_BASE(Id);
    for (eapi_gpio_id = EAPI_GPIO_BANK_GPIO_ID_BASE(Id), idx = 0;
            idx < EAPI_GPIO_NUMBER_PER_SET;
            eapi_gpio_id++, idx++)
    {
        gpio_id = eapi_gpio_id;
        if ((BitMask & (1 << idx)) && (gpio_id < 0x8)) {
            StatusCode = single_gpio_set_dir(gpio_id, (uint32_t)((Direction & (0x1 << idx)) >> idx));
        }
    }

    return StatusCode;
}

EApiStatus_t gpio_set_dir(__IN EApiId_t Id, __IN uint32_t BitMask, __IN uint32_t Direction) {

	EApiStatus_t StatusCode = EAPI_STATUS_UNSUPPORTED;
    	uint32_t gpio_id;

    	DBG("gpio_set_dir Id:%d\n", Id);
    	DBG("Id:%d, BitMask=0x%x, Direction=0x%x\n", Id, BitMask, Direction);

    	if (IS_BANK_ID(Id)) StatusCode = bank_gpio_set_dir(Id, BitMask, Direction);
    	else {
        	gpio_id = Id;
        	StatusCode = single_gpio_set_dir(gpio_id, Direction);
    	}

    	DBG("StatusCode = 0x%x\n", StatusCode);
    	return StatusCode;
}

static EApiStatus_t bank_gpio_get_dir_caps(uint32_t eapi_gpio_bank_id __attribute__ ((unused)), uint32_t *pInputs, uint32_t *pOutputs) {

	EApiStatus_t StatusCode = EAPI_STATUS_UNSUPPORTED;
    	uint32_t caps = 0, capMask = 0;

    	if (!IS_BANK_ID(eapi_gpio_bank_id)) {
        	StatusCode = EAPI_STATUS_INVALID_PARAMETER;
        	DBG("EAPI_STATUS_INVALID_PARAMETER: eapi_gpio_bank_id=0x%x\n", eapi_gpio_bank_id);
        	goto failure;
    	}
    	
    	caps = gpio_count-1;
    	if ((caps >= 32) || (caps <= 0)) { // No avaliable pins
        	DBG("Error at call_method(): caps=0x%x\n", caps);
        	goto failure;
    	}
    	while (caps) {
        	capMask |= (0x1 << caps);
        	caps--;
    	}
   
        *pInputs = capMask+1;

	*pOutputs = capMask+1;

    	StatusCode = EAPI_STATUS_SUCCESS;
    	DBG("bank_gpio_get_dir_caps input bitmap = %x, output bitmap = %x\n", *pInputs, *pOutputs);

failure:
    return StatusCode;
}

static EApiStatus_t single_gpio_get_dir_caps(uint32_t eapi_gpio_id, uint32_t *pInputs, uint32_t *pOutputs) {

	EApiStatus_t StatusCode = EAPI_STATUS_UNSUPPORTED;
    	uint32_t capInputs = 0, capOutputs = 0;

    	if (IS_BANK_ID(eapi_gpio_id)) {
        	StatusCode = EAPI_STATUS_INVALID_PARAMETER;
        	DBG("EAPI_STATUS_INVALID_PARAMETER: eapi_gpio_id=0x%x\n", eapi_gpio_id);
        	goto failure;
    	}

    	StatusCode = bank_gpio_get_dir_caps(EAPI_GPIO_BANK_ID(eapi_gpio_id), &capInputs, &capOutputs);
    	
    	if (StatusCode != EAPI_STATUS_SUCCESS) {
        	DBG("Error to get capabilities: 0x%x\n", StatusCode);
        	goto failure;
    	}

	DBG("%d\n", (eapi_gpio_id & 0x1F));
    	*pInputs = BIT_VALUE(capInputs, (eapi_gpio_id & 0x1F));
    	DBG("pInputs:%x\n", *pInputs);
    	*pOutputs = BIT_VALUE(capOutputs, (eapi_gpio_id & 0x1F));
    	DBG("pOutputs:%x\n", *pOutputs);

    	StatusCode = EAPI_STATUS_SUCCESS;

failure:
    	return StatusCode;
}

EApiStatus_t gpio_get_dir_caps(__IN EApiId_t Id, __OUTOPT uint32_t *pInputs, __OUTOPT uint32_t *pOutputs) {

	EApiStatus_t StatusCode = EAPI_STATUS_UNSUPPORTED;
	
	if (IS_BANK_ID(Id)) {
		StatusCode = bank_gpio_get_dir_caps(Id, pInputs, pOutputs);
    	} else {
    		StatusCode = single_gpio_get_dir_caps(Id, pInputs, pOutputs);
    	}

    	return StatusCode;
}

EApiStatus_t gpio_get_count(__OUT uint32_t *pCount) {

	EApiStatus_t StatusCode = EAPI_STATUS_UNSUPPORTED;
	
	if(gpio_pinnum != 0) {
		*pCount = gpio_count;
		StatusCode = EAPI_STATUS_SUCCESS;
	}
	
	return StatusCode;
}
	
	
	
	
		



