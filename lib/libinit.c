#include <EApiLib.h>
#include <errno.h>

#if SUPPORT_STORGE
#include <hook_storage.h>
#include <eapi_storage_impl.h>
#endif //SUPPORT_STORGE

#include <eapi_boardinfo_impl.h>
#include <hook_boardinfo.h>

#if SUPPORT_GPIO
#include <eapi_gpio_impl.h>
#include <hook_gpio.h>
#endif //SUPPORT_GPIO

#if SUPPORT_WDT
#include <eapi_watchdog_impl.h>
#include <hook_watchdog.h>
#endif //SUPPORT_WDT

#if SUPPORT_BLACK_LIGHT
#include <eapi_backlight_impl.h>
#include <hook_backlight.h>
#endif //SUPPORT_BLACK_LIGHT

#include <libinit.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define BOARD_VENDOR	"AAEON"
#define AAEON_STRING_START_ADDR 0xF0000
#define MEMORY_MAPPING_SIZE 0x10000
static __inline__ bool is_aaeon_platform(void) {

	bool ret = false;
    	int nFound = 1;
    	FILE *input=NULL;
    	char buf[30];
	char line[30];
    	char *p=NULL;
	char buffer1[] = {0x22, 0x34, 0x03, (char)0x98, 'A', 'A', 'E', 'O', 'N'};
		
	int fd = open("/dev/mem", O_RDWR | O_SYNC);
	
	if(fd < 0) {
		DBG("device file open error\n");
		goto exit;
	
	}
	
	void *mapped = mmap(NULL, MEMORY_MAPPING_SIZE, PROT_READ, MAP_SHARED, fd, AAEON_STRING_START_ADDR);
	close(fd);
	if(mapped == MAP_FAILED) {
		DBG("Can't map memory\n");
		goto exit;
	}
	
	for(size_t noffset = 0; noffset < MEMORY_MAPPING_SIZE; noffset+=sizeof(buffer1)) {
    		nFound = memcmp((const void *)(mapped+noffset), (const void *) buffer1, sizeof(buffer1));
    		if(nFound == 0) {
    			ret = true;
    			munmap(mapped, MEMORY_MAPPING_SIZE);
    			goto exit;
    		}	
    	}
    	    	
   	munmap(mapped, MEMORY_MAPPING_SIZE);
	
	system("dmidecode -t 1 | grep Manufacturer | awk '{print $2}' >> boardis.txt");
	input = fopen("boardis.txt", "r");
	memset(buf, 0, 30);
	memset(line, 0, 30);
	
	while (fgets(line, sizeof(line), input) != NULL) strcpy( buf, line);
	
	fclose(input);
	
	p=strrchr(buf, '\n');/* search last space */
    	*p = '\0';/* split at last space */
	
	nFound = strcmp(BOARD_VENDOR, buf);
	if(nFound == 0) ret = true;
			
	system("sudo rm -rf boardis.txt");	
	
	    	
exit:
    	return ret;
}

EApiStatus_t libinit_init(void) {

	EApiStatus_t StatusCode = EAPI_STATUS_UNSUPPORTED;
    	//uint32_t  sub_id = 0;
        
    	if (!is_aaeon_platform()) {
    		goto libinit_fail;
    	} 

    	// Regiter Boardinfo Function Hook
    	struct eapi_boardinfo_func boardinfo_func = {
        	.init = boardinfo_aaeon_init,
        	.deinit = boardinfo_aaeon_deinit,
        	.get_value = boardinfo_aaeon_get_value,
        	.get_string_a = boardinfo_aaeon_get_string_a
    	};

    	register_eapi_boardinfo_func(boardinfo_func);
#if SUPPORT_SMART_FAN
    	// Register Smart Fan Function Hook
    	struct eapi_smartfan_func smartfan_func = {
        	.init = fanctrl_init,
        	.deinit = fanctrl_deinit,
        	.get_fanspeed = fanctrl_get_speed,
        	.set_fanspeed = fanctrl_set_speed,
        	.get_fanmode = fanctrl_get_mode,
        	.set_fanmode = fanctrl_set_mode,
    	};

    	register_eapi_smartfan_func(smartfan_func);
#endif //SUPPORT_SMART_FAN

#if SUPPORT_BLACK_LIGHT
    	// Register Backlight Function Hook
    	struct eapi_backlight_func backlight_func = {
        	.init = blctrl_init,
        	.deinit = blctrl_deinit,
        	.get_enable = blctrl_get_enable,
        	.set_enable = blctrl_set_enable,
        	.get_brightness = blctrl_get_brightness,
        	.set_brightness = blctrl_set_brightness,
    	};
    	register_eapi_backlight_func(backlight_func);

#endif	//SUPPORT_BLACK_LIGHT

#if SUPPORT_STORGE
    	// Register Storage Function Hook
    	struct eapi_storage_func storage_func = {
        	.init = NULL,
        	.deinit = NULL,
        	.write = NULL,
        	.read = NULL,
        	.get_cap = NULL,
    	};
    	register_eapi_storage_func(storage_func);
#endif	//SUPPORT_STORGE

#if SUPPORT_GPIO
    	// Register GPIO Function Hook
    	struct eapi_gpio_func gpio_func = {
        	.init = gpio_init,
        	.deinit = gpio_deinit,
        	.get_lv = gpio_get_lv,
        	.set_lv = gpio_set_lv,
        	.get_dir = gpio_get_dir,
        	.set_dir = gpio_set_dir,
        	.get_dir_caps = gpio_get_dir_caps,
        	.get_count = gpio_get_count,
    	};
    	register_eapi_gpio_func(gpio_func);
#endif	//SUPPORT_GPIO

#if SUPPORT_WDT
    	// Register Watchdog Function Hook
    	struct eapi_watchdog_func watchdog_func = {
        	.init = watchdog_aaeon_init,
        	.deinit = watchdog_aaeon_deinit,
        	.get_cap = watchdog_aaeon_get_cap,
        	.start = watchdog_aaeon_start,
        	.trigger = watchdog_aaeon_trigger,
        	.stop = watchdog_stop,
        	
    	};

    	register_eapi_watchdog_func(watchdog_func);

#endif	//SUPPORT_WDT
#if SUPPORT_ALERT
    	// Register Alert Function Hook
    	struct eapi_alert_func alert_func = {
        	.init = NULL,
        	.deinit = NULL,
        	.get_cap = NULL,
        	.get_value = NULL,
        	.set_value = NULL,
        	.check = NULL,
    	};

    	register_eapi_alert_func(alert_func);
#endif //SUPPORT_ALERT

	StatusCode = EAPI_STATUS_SUCCESS;
libinit_fail:
    return StatusCode;
}

EApiStatus_t libinit_deinit(void) {

	EApiStatus_t StatusCode = EAPI_STATUS_SUCCESS;
    	return StatusCode;
}
