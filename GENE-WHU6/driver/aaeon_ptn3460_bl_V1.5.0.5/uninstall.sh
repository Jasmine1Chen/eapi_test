#!/bin/sh

title()
{
	echo -----------------------------------------
	echo "Uninstall PTN3460 Backlight Driver."
	echo -----------------------------------------
}

show_usage()
{
	echo "Usage:"
	echo "		./uninstall.sh"
	echo
	echo "Ex:"
	echo "		command: ./uninstall.sh"
	echo
}

title

echo "Check the old driver and unload it."

check=`lsmod | grep -w ptn3460`
if [ "$check" != "" ]; then
    echo "Unload ptn3460"
    sudo /sbin/rmmod ptn3460
fi

echo "Build the module and uninstall."
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
	OLD_DRIVER=$(find /lib/modules/$(uname -r)/kernel/drivers/gpu/drm/ -iname ptn3460.ko -type f)
	BAK_DRIVER=$(find /lib/modules/$(uname -r)/kernel/drivers/gpu/drm/ -iname ptn3460.ko_bak -type f)
	TARGET_PATH=/lib/modules/$(uname -r)/kernel/drivers/gpu/drm/
	
	if [ "$BAK_DRIVER" = "" ]; then
	
		if [ "$OLD_DRIVER" != "" ]; then
			sudo rm -rf $TARGET_PATH/ptn3460.ko
		fi
	
	else

		sudo rm -rf $TARGET_PATH/ptn3460.ko*

	fi
fi

echo "Remove Config driver module..."
if [ -f /etc/modules-load.d/ptn3460.conf ]; then
	sudo rm -rf /etc/modules-load.d/ptn3460.conf
fi

echo "Update driver."
sudo depmod -a

echo "Uninstallation Completed. "
exit 0

