bash ./formatFiles.sh
echo $files
for device in m5stickc m5stickcplus tdisplayt4 tdisplayremote fireremote homeThingTTGO
do
    echo building $device
    esphome run --no-logs $device.yaml
done
