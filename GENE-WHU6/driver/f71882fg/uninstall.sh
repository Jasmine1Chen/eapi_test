#!/bin/sh

title()
{
	echo -----------------------------------------
	echo "UnInstall F71882fg Hardware Monitor Driver."
	echo -----------------------------------------
}

title

echo "Check old driver and unload it."
check=`lsmod | grep -w f71882fg`
if [ "$check" != "" ]; then
	echo "rmmod f71882fg"
	sudo /sbin/rmmod f71882fg
fi


echo "Search the module and uninstall."
echo "-------------------------------" >> log.txt
date 1>>log.txt
make all 1>>log.txt || exit 1

OLD_DRIVER=$(find /lib/modules/$(uname -r)/kernel/drivers/hwmon -iname f71882fg.ko -type f)
TARGET_PATH=/lib/modules/$(uname -r)/kernel/drivers/hwmon
	
if [ "$OLD_DRIVER" != "" ]; then
	sudo rm -rf $TARGET_PATH/f71882fg.*
fi

echo "Update driver."
sudo depmod -a

echo "Completed."
exit 0

