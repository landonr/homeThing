echo "formatting"
bash formatFiles.sh
echo "building"
esphome run homething-lilygo.yaml --device /dev/ttyACM0
