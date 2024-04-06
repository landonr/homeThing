#!/bin/bash

format_files() {
    echo "formatting"
    bash formatFiles.sh &> /dev/null
}

build_devices() {
    local device=$1
    echo "building $device"
    esphome run --no-logs "$device.yaml"
}

build_with_usb_device() {
    local device=$1
    local usb_device=$2
    echo "building $device = $usb_device"
    esphome run "$device" --device "$usb_device"
}

build_default_with_usb_device() {
    local default_device=$1
    local usb_device=$2
    echo "building default $default_device = $usb_device"
    esphome run "$default_device" --device "$usb_device"
}

build_without_usb() {
    local device=$1
    echo "no usb building $device"
    esphome run "$device"
}

build_default_without_usb() {
    local default_device=$1
    echo "no usb building default $default_device"
    esphome run "$default_device"
}

update_yaml_branch() {
    local yaml_file="$1"
    local branch_name="$2"
    local temp_file="$yaml_file.tmp" # Create a temporary file
    sed "s|@main \#update_to_head_ref|@$branch_name|g" "$yaml_file" > "$temp_file"
    echo "$temp_file"
}

detect_usb_devices() {
    local yaml_file="$1"
    local has_tty_usb=0
    local more_than_one=0
    local tty_usb_device=""

    if compgen -G "/dev/ttyUSB*" > /dev/null; then
        for i in /dev/ttyUSB*; do
            if [ "$has_tty_usb" -eq 1 ]; then 
                more_than_one=1
            fi
            has_tty_usb=1
            tty_usb_device=$i
            echo "USB=$tty_usb_device"
        done
    elif compgen -G "/dev/ttyACM*" > /dev/null; then
        for i in /dev/ttyACM*; do
            if [ "$has_tty_usb" -eq 1 ]; then 
                more_than_one=1
            fi
            has_tty_usb=1
            tty_usb_device=$i
            echo "USB=$tty_usb_device"
        done
    elif compgen -G "/dev/cu.usbserial-*" > /dev/null; then
        for i in /dev/cu.usbserial-*; do
            has_tty_usb=1
            tty_usb_device=$i
            echo "USB=$tty_usb_device"
        done
    fi

    if [ "$has_tty_usb" -eq 1 ]; then
        if [ ! -z "$2" ]; then
            build_with_usb_device "$2" "$tty_usb_device"
        else
            build_default_with_usb_device "$yaml_file" "$tty_usb_device"
        fi
    elif [ "$more_than_one" -eq 1 ]; then
        echo "more than one usb port, can you specify which"
    else
        if [ ! -z "$2" ]; then
            build_without_usb "$2"
        else
            build_default_without_usb "$yaml_file"
        fi
    fi
}

main() {
    format_files

    if [ -z "$1" ]; then
        echo "YAML file parameter is missing"
        exit 1
    fi

    local yaml_file="$1"

    if [ ! -f "$yaml_file" ]; then
        echo "YAML file not found: $yaml_file"
        exit 1
    fi

    if [ ! -z "$2" ]; then
        if [ "$2" == "all" ]; then
            for device in tdisplay-ipod tdisplay-megadesk tdisplay-t4 m5stack-fire m5stack-stickc m5stack-stickcplus-rotary tdisplay-s3; do
                build_devices "$device"
            done
            exit 0
        else
            echo "didn't work: $2"
            exit 1
        fi
    fi

    # Get current branch name
    local branch_name=$(git rev-parse --abbrev-ref HEAD)

    # Update YAML file and get temporary file path
    local temp_yaml_file=$(update_yaml_branch "$yaml_file" "$branch_name")

    detect_usb_devices "$yaml_file" "$temp_yaml_file"

    # Remove temporary file after building
    rm "$temp_yaml_file"
}

main "$@"
