#!/bin/bash

if [[ `whoami` != "root" ]]; then
    echo "Should run in root"
    exit 1
fi

if ldd `which date` | grep -q "vdso"; then
    echo "vDSO is on"
else
    echo "vDSO is already off"
    exit 0
fi

echo -n "It will automatically modify Grub config file, do you want to proceed? [y/N]:"
read ans

if [[ ! (($ans == "y" || $ans == "Y")) ]]; then
    echo "You should modify the file manually later to enable vDSO"
    exit 0
fi

GRUB_CONFIG_FILE_PATH="/etc/default/grub"
GRUB_CONFIG_BACKUP_FILE_NAME="grub.backup"

if [[ ! -e $GRUB_CONFIG_FILE_PATH ]]; then
    echo "GRUB config file $GRUB_CONFIG_FILE_PATH not exists."
    exit 1
fi

CONFIG_LINE=`cat $GRUB_CONFIG_FILE_PATH | grep "GRUB_CMDLINE_LINUX_DEFAULT"`

if [[ -z "$CONFIG_LINE" ]]; then
    echo "Grub config GRUB_CMDLINE_LINUX_DEFAULT is not found in file $GRUB_CONFIG_FILE_PATH."
    exit 1
fi

if echo "$CONFIG_LINE" | grep -q "vdso"; then
    echo "vDSO has been already set in $CONFIG_LINE in file $GRUB_CONFIG_FILE_PATH"
    exit 1
fi

NEW_CONFIG_LINE=`echo "$CONFIG_LINE" | sed "s/GRUB_CMDLINE_LINUX_DEFAULT=\"/&vdso=0 /"`

echo "Will modify config in $GRUB_CONFIG_FILE_PATH to this:"
echo $NEW_CONFIG_LINE
echo -n ", do you want to proceed? [y/N]:"
read ans

if [[ ! (($ans == "y" || $ans == "Y")) ]]; then
    echo "You should modify the file manually later to enable vDSO"
    exit 0
fi

counter=0
while true
do
    if [[ $counter -ne 0 ]]; then
        target="$GRUB_CONFIG_BACKUP_FILE_NAME.$counter"
    else
        target=$GRUB_CONFIG_BACKUP_FILE_NAME
    fi
    if [[ ! -e $target ]]; then
        echo "Copying $GRUB_CONFIG_FILE_PATH to $target"
        cp $GRUB_CONFIG_FILE_PATH $target
        break
    fi
    ((counter++))
done

sed -i "s/GRUB_CMDLINE_LINUX_DEFAULT=\"/&vdso=0 /" $GRUB_CONFIG_FILE_PATH

echo "Running grub-mkconfig"
grub-mkconfig -o /boot/grub/grub.cfg
echo "Finished grub-mkconfig"

echo -n "Need reboot to disable vDSO, do you want to reboot now? [y/N]:"
read ans

if [[ $ans == "y" || $ans == "Y" ]]; then
    echo "Reboot now"
    # reboot
else
    echo "You should reboot manually later to enable vDSO"
    exit 0
fi
