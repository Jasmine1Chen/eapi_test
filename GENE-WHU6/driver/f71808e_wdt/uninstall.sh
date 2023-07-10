#!/bin/sh

title()
{
	echo -----------------------------------------
	echo "UnInstall f71808e Watch Dog Driver."
	echo -----------------------------------------
}

title

echo "Check old driver and unload it."
check=`lsmod | grep -w f71808e_wdt`
if [ "$check" != "" ]; then
	echo "rmmod f71808e_wdt"
	sudo /sbin/rmmod f71808e_wdt
fi


echo "Search the module and uninstall."
echo "-------------------------------" >> log.txt
date 1>>log.txt
make all 1>>log.txt || exit 1

OLD_DRIVER=$(find /lib/modules/$(uname -r)/kernel/drivers/watchdog -iname f71808e_wdt.ko -type f)
TARGET_PATH=/lib/modules/$(uname -r)/kernel/drivers/watchdog
	
if [ "$OLD_DRIVER" != "" ]; then
	sudo rm -rf $TARGET_PATH/f71808e_wdt.*
fi

echo "Update driver."
sudo depmod -a

echo "Completed."
exit 0

