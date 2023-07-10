#!/bin/sh

title()
{
	echo -----------------------------------------
	echo "Install F71808e Watch Dog Driver."
	echo -----------------------------------------
}

title

echo "Check old driver and unload it."
check=`lsmod | grep -w f71808e_wdt`
if [ "$check" != "" ]; then
	echo "rmmod f71808e_wdt"
	sudo /sbin/rmmod f71808e_wdt
fi


echo "Build the module and install."
echo "-------------------------------" >> log.txt
date 1>>log.txt
make all 1>>log.txt || exit 1

module=`ls driver/*.ko`
module=${module#driver/}
module=${module%.ko}

if [ "$module" = "" ]; then
	echo "No driver exists!!!"
	exit 1
else
	OLD_DRIVER=$(find /lib/modules/$(uname -r)/kernel/drivers/watchdog -iname f71808e_wdt.ko -type f)
	BAK_DRIVER=$(find /lib/modules/$(uname -r)/kernel/drivers/watchdog -iname f71808e_wdt.ko_bak -type f)
	TARGET_PATH=/lib/modules/$(uname -r)/kernel/drivers/watchdog
	
	if [ "$BAK_DRIVER" = "" ]; then
	
		if [ "$OLD_DRIVER" = "" ]; then
			sudo cp driver/f71808e_wdt.ko $TARGET_PATH/
		else
			sudo mv $OLD_DRIVER $TARGET_PATH/f71808e_wdt.ko_bak
			sudo cp driver/f71808e_wdt.ko $TARGET_PATH/
		fi
	
	else
		
		sudo cp driver/f71808e_wdt.ko $TARGET_PATH/
		
	fi
fi

echo "Update driver."
sudo depmod -a

echo "Load module: $module ."
sudo modprobe $module 

echo "Completed."
exit 0

