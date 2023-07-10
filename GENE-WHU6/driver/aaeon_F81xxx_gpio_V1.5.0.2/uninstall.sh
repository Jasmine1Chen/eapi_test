#!/bin/sh

title()
{
	echo -----------------------------------------
	echo "UnInstall gpio-fxxx GPIO Driver."
	echo -----------------------------------------
}

title

echo "Check old driver and unload it."
check=`lsmod | grep -w gpio-fxxx`
if [ "$check" != "" ]; then
	echo "rmmod gpio-fxxx"
	sudo /sbin/rmmod gpio-fxxx
fi


echo "Search the module and uninstall."
echo "-------------------------------" >> log.txt
date 1>>log.txt
make all 1>>log.txt || exit 1

OLD_DRIVER=$(find /lib/modules/$(uname -r)/kernel/drivers/gpio -iname gpio-fxxx.ko -type f)
TARGET_PATH=/lib/modules/$(uname -r)/kernel/drivers/gpio
	
if [ "$OLD_DRIVER" != "" ]; then
	sudo rm -rf $TARGET_PATH/gpio-fxxx.*
fi

if [ -f /etc/modules-load.d/fxxx_gpio.conf ]; then
	sudo rm -rf /etc/modules-load.d/fxxx_gpio.conf
fi

echo "Update driver."
sudo depmod -a

echo "Completed."
exit 0
