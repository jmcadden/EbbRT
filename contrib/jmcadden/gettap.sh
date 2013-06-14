#!/bin/bash
NAME=$USER
ID={id -u}

ifconfig -s
echo "The interfaces listed above are currently active."
echo "Which tap interface number would you like to create?"
read -e NUM
if [[ $NUM != *[!0-9]* ]]; then
  echo "Attempting to create tap$NUM"
else
  echo "'$NUM' has a non-digit somewhere in it"
  exit 0
fi

sudo tunctl -u $NAME -t tap$NUM
sudo ifconfig tap$NUM 192.168.0.$NUM up
#sudo route add -host 192.168.$NUM.253 dev tap$NUM
#sudo echo 1 > /proc/sys/net/ipv4/conf/tap$NUM/proxy_arp
#sudo apr -Ds 192.168.0.253 eth0 pub

# generate a random mac address for qemu 
#printf 'DE:AD:BE:EF:%02X:%02X\n' $((RANDOM%256)) $((RANDOM%256))
ifconfig -s


echo "This interface is persistant and will remain active until you close it"
echo "To close it, run the following: sudo tunctl -d tab$NUM"
