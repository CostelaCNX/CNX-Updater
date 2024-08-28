#/bin/bash
echo ""
echo "******************* Creating the release ZIP..."
fVar=$(find -type f -name '*-updater.nro');
fT=${fVar:2}
mkdir switch
mkdir ./switch/${fT%.*}
cp $fT ./switch/${fT%.*}
zip -r -9 ./${fT%.*}.zip ./switch/
rm -rf switch

echo ""
echo "******************* Done!"