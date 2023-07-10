#!/bin/sh

title()
{
	echo -----------------------------------------
	echo "Install PTN3460 Backlight Driver."
	echo -----------------------------------------
}

show_usage()
{
	echo "Usage:"
	echo "		./install.sh"
	echo
	echo "Ex:"
	echo "		command: ./install.sh"
	echo
}

title

echo "Check the old driver and unload it."

check=`lsmod | grep -w ptn3460`
if [ "$check" != "" ]; then
    echo "Unload ptn3460"
    sudo /sbin/rmmod ptn3460
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
	OLD_DRIVER=$(find /lib/modules/$(uname -r)/kernel/drivers/gpu/drm/ -iname ptn3460.ko -type f)
	BAK_DRIVER=$(find /lib/modules/$(uname -r)/kernel/drivers/gpu/drm/ -iname ptn3460.ko_bak -type f)
	TARGET_PATH=/lib/modules/$(uname -r)/kernel/drivers/gpu/drm/
	
	if [ "$BAK_DRIVER" = "" ]; then
	
		if [ "$OLD_DRIVER" = "" ]; then
			sudo cp driver/ptn3460.ko $TARGET_PATH/
		else
			sudo mv $OLD_DRIVER $TARGET_PATH/ptn3460.ko_bak
			sudo cp driver/ptn3460.ko $TARGET_PATH/
		fi
	
	else

		sudo cp driver/ptn3460.ko $TARGET_PATH/

	fi
fi

echo "Config driver module..."
sudo su
if [ ! -f /etc/modules-load.d/ptn3460.conf ]; then
cat <<EOF > /etc/modules-load.d/ptn3460.conf
ptn3460
EOF
fi

echo "Update driver."
sudo depmod -a

echo "Load module."
sudo modprobe i2c_i801
sudo modprobe ptn3460

echo "Installation Completed. Please reboot to apply the driver."
exit 0

