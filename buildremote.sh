echo "formatting"
bash formatFiles.sh &> /dev/null

defaultDevice="homething-lilygo.yaml"
hasTTYUSB=0
moreThanOne=0
ttyUSBDevice=""

if [ ! -z $1 ]; then
    if [ $1 == "all" ]; then
        for device in tdisplay-ipod tdisplay-megadesk tdisplay-t4 m5stack-fire m5stack-stickc m5stack-stickcplus-rotary tdisplay-s3
        do
            echo building $device
            esphome run --no-logs $device.yaml
        done
        exit 0
    fi
    echo didnt work1 $1
fi

if compgen -G "/dev/ttyUSB*" > /dev/null;
then
    for i in /dev/ttyUSB* 
    do
        if [ "$hasTTYUSB" -eq 1 ]; then 
            moreThanOne=1
        fi
        hasTTYUSB=1
        ttyUSBDevice=$i
        echo "USB="$ttyUSBDevice
    done  
elif compgen -G "/dev/ttyACM*" > /dev/null;
then
    for i in /dev/ttyACM* 
    do
        if [ "$hasTTYUSB" -eq 1 ]; then 
            moreThanOne=1
        fi
        hasTTYUSB=1
        ttyUSBDevice=$i
        echo "USB="$ttyUSBDevice
    done
elif compgen -G "/dev/cu.usbserial-*" > /dev/null;
then
    for i in /dev/cu.usbserial-* 
    do
        hasTTYUSB=1
        ttyUSBDevice=$i
        echo "USB="$ttyUSBDevice
    done
fi

if [ "$hasTTYUSB" -eq 1 ]; then
    if [ ! -z $1 ]; then
        echo "building $1 =" $ttyUSBDevice
        esphome run $1 --device $ttyUSBDevice
    else
        echo "building default $defaultDevice =" $ttyUSBDevice
        esphome run $defaultDevice --device $ttyUSBDevice
    fi
elif [ "$moreThanOne" -eq 1 ]; then
    echo "more than one usb port, can you specify which"
else
    if [ ! -z $1 ]; then
        echo "no usb building $1"
        esphome run $1
    else
        echo "no usb building default $defaultDevice"
        esphome run $defaultDevice
    fi
fi
