#/bin/bash
clear
echo ""
echo "****************************************************************"
echo "*                        SCRIPT STARTED                        *"
echo "****************************************************************"
echo ""

if [[ $# -gt 0 ]] ; then
    echo "******************* Updating from GitHub (DISCARDING ALL LOCAL CHANGES)..."
    git fetch --all
    git reset --hard origin/main
    git pull origin main
    echo ""
fi

echo "******************* Building forwarder..."
cd app-forwarder
make
cd ..
echo ""

echo "******************* Building application..."
make
echo ""

./release.sh

echo ""

echo "****************************************************************"
echo "*                       SCRIPT TERMINATED                      *"
echo "****************************************************************"
