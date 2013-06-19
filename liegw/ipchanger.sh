#!/bin/bash

PATH=/home/anre
FILENAME=interface
FILE=$PATH/$FILENAME
MV=/bin/mv

DHCP="dhcp"
STATIC="static"

#save old config
$MV $PATH/interface $PATH/interface.old

if [ "$1" = "$DHCP" ];then
  echo "set IP to dhcp"
elif [ "$1" = "$STATIC" ];then
  echo "set static IP"
  if [ -z "$2" -o -z "$3" -o -z "$4" ];then
    echo "usage: ipchanger.h static address netmask gateway"
    exit 1
  fi
fi


echo "auto lo" > $FILE
echo "iface lo inet loopback" >> $FILE
echo "# The \"pre-up\" lines are to stop a WEMAC NIC from being configured in case " >> $FILE
echo "# there are other non-WEMAC NICs in the system. This is necessary to avoid " >> $FILE
echo "# using WEMAC on devices with no built-in Ethernet like the MK802. " >> $FILE
echo " " >> $FILE
echo "# If you use a device with built-in Ethernet and also use an external NIC " >> $FILE
echo "# (e.g. for a router usage scenario), feel free to remove these pre-up lines " >> $FILE
echo "# so that both NICs are used. " >> $FILE
echo " " >> $FILE
echo "# If you use an MK802 as WiFi-only (setting up wlan0 and disconnecting " >> $FILE
echo "# USB Ethernet), remove both eth0/eth1 sections entirely. " >> $FILE
echo " " >> $FILE
echo "auto eth0 " >> $FILE

if [ "$1" = "$DHCP" ];then
  echo "iface eth0 inet dhcp " >> $FILE
  echo "  pre-up /usr/local/bin/eni-pre-up.sh eth0 " >> $FILE
elif [ "$1" = "$STATIC" ]; then
  echo "iface eth0 inet static" >> $FILE
  echo "  pre-up /usr/local/bin/eni-pre-up.sh eth0 " >> $FILE
  echo "  address $2" >> $FILE
  echo "  netmask $3" >> $FILE
  echo "  gateway $4" >> $FILE
fi

echo "iface eth0 inet6 static " >> $FILE
echo "  address 2001:15c0:666d:100::1 " >> $FILE
echo "  netmask 64" >> $FILE
echo " " >> $FILE
echo "auto eth1" >> $FILE
echo "iface eth1 inet dhcp" >> $FILE
echo "  pre-up /usr/local/bin/eni-pre-up.sh eth1" >> $FILE
echo " " >> $FILE

