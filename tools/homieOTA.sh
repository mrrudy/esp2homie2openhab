#!/bin/sh


#echo `echo "${1} a potem '\\$1'"`;
#exit;
mosquitto_pub -h 192.168.1.25 -m `md5sum ${1} | awk '{print \$1}'` -t "${2}/\$implementation/ota/checksum"; 
base64 -w0 ${1} > "${1}.base64";
mosquitto_pub -h 192.168.1.25 -d -f "${1}.base64" -t "${2}/\$implementation/ota/firmware"
