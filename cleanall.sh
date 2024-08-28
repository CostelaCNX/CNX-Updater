#/bin/bash
clear
echo ""
echo "****************************************************************"
echo "*                        SCRIPT STARTED                        *"
echo "****************************************************************"
echo ""
echo "******************* Cleaning environment..."
make clean

echo "cleaning forwarder..."
cd app-forwarder
rm -rf build
rm app-forwarder.elf
rm app-forwarder.nacp
rm app-forwarder.nro
cd ..

echo "cleaning app-rcm..."
cd app-rcm
rm -rf build
rm -rf output
cd ..

echo "cleaning resources..."
cd resources
rm -rf app_rcm.bin
rm -rf app_forwarder.nro
cd ..

echo "finishing..."
rm -rf *-updater.*


echo ""
echo "****************************************************************"
echo "*                       SCRIPT TERMINATED                      *"
echo "****************************************************************"
