#!/usr/bin/env bash
cd "$(dirname "${BASH_SOURCE[0]}")"

if [[ "$EUID" -ne 0 ]]; then
    echo "This script must be run as root."
    exit 1
fi

id -u "$1" > /dev/null 2>&1
if [[ $? -ne 0 ]]; then
    echo "The user '$1' does not exist."
    exit 2
fi

echo "Copying udev rule to system..."
cp -f ./77-controlloid-uinput.rules /etc/udev/rules.d/

echo "Creating group 'controlloid'..."
groupadd -f controlloid

echo "Adding user '$1' to the group..."
usermod -a -G controlloid $1

echo "Done! Please reboot your machine."
