#!/bin/sh

title()
{
	echo -----------------------------------------
	echo "Install F71882fg Hardware Monitor Driver."
	echo -----------------------------------------
}

title

echo "Check old driver and unload it."
check=`lsmod | grep -w f71882fg`
if [ "$check" != "" ]; then
	echo "rmmod f71882fg"
	sudo /sbin/rmmod f71882fg
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
	OLD_DRIVER=$(find /lib/modules/$(uname -r)/kernel/drivers/hwmon -iname f71882fg.ko -type f)
	BAK_DRIVER=$(find /lib/modules/$(uname -r)/kernel/drivers/hwmon -iname f71882fg.ko_bak -type f)
	TARGET_PATH=/lib/modules/$(uname -r)/kernel/drivers/hwmon
	
	if [ "$BAK_DRIVER" = "" ]; then
	
		if [ "$OLD_DRIVER" = "" ]; then
			sudo cp driver/f71882fg.ko $TARGET_PATH/
		else
			sudo mv $OLD_DRIVER $TARGET_PATH/f71882fg.ko_bak
			sudo cp driver/f71882fg.ko $TARGET_PATH/
		fi
	
	else
		
		sudo cp driver/f71882fg.ko $TARGET_PATH/
		
	fi
fi

echo "Update driver."
sudo depmod -a

echo "Load module: $module ."
sudo modprobe $module 

echo "Completed."
exit 0

