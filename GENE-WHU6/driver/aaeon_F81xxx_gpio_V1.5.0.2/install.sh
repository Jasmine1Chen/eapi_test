#!/bin/sh

title()
{
	echo -----------------------------------------
	echo "Install AAEON Fxxx GPIO Driver."
	echo -----------------------------------------
}

show_usage()
{
	echo "install.sh <chip name number [0-2]>\n"
	echo "0: f75113\n"
	echo "1: f81866\n"
	echo "2: f81966\n"
	echo "3: f81801\n"
}

if [ \( $# -eq 0 \) -o \( $# -gt 1 \) ] ; then
    show_usage
    echo "ERROR: incorrect number of parameters\n"
    exit 255
fi

title

echo "Check old driver and unload it."
check=`lsmod | grep -w gpio_fxxx`
if [ "$check" != "" ]; then
	echo "rmmod gpio-fxxx"
	sudo /sbin/rmmod gpio-fxxx
fi

check=`lsmod | grep -w gpio_aaeon`
if [ "$check" != "" ]; then
	echo "rmmod gpio_aaeon"
	sudo /sbin/rmmod gpio_aaeon
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
	OLD_DRIVER=$(find /lib/modules/$(uname -r)/kernel/drivers/gpio -iname gpio-fxxx.ko -type f)
	BAK_DRIVER=$(find /lib/modules/$(uname -r)/kernel/drivers/gpio -iname gpio-fxxx.ko_bak -type f)
	TARGET_PATH=/lib/modules/$(uname -r)/kernel/drivers/gpio
	
	if [ "$BAK_DRIVER" = "" ]; then
	
		if [ "$OLD_DRIVER" = "" ]; then
			sudo cp driver/gpio-fxxx.ko $TARGET_PATH/
		else
			sudo mv $OLD_DRIVER $TARGET_PATH/gpio-fxxx.ko_bak
			sudo cp driver/gpio-fxxx.ko $TARGET_PATH/
		fi
	
	else
		
		sudo cp driver/gpio-fxxx.ko $TARGET_PATH/
		
	fi
fi

echo "Update driver."
sudo depmod -a

echo "Load module: $module ."
sudo modprobe $module chip_num=$1

#Add modprobe to init

echo "Add driver to init."

if [ ! -f /etc/modules-load.d/fxxx_gpio.conf ]; then
cat <<EOF > /etc/modules-load.d/fxxx_gpio.conf
gpio-fxxx
EOF
fi

if [ ! -f /etc/modprobe.d/fxxx_gpio.conf ]; then
cat <<EOF > /etc/modprobe.d/fxxx_gpio.conf
options gpio-fxxx chip_num=$1
EOF
fi

echo "Completed."
exit 0

