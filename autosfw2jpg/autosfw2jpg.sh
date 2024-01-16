#!/bin/bash
#
# A tool to help convert and copy all .sfw image files from a Seattle Film Works CDROM.
#
# Author: Dean Gadberry https://github.com/DeanGadberry
# Date: 15 January 2024
#
# For more information see https://github.com/jasonacox/sfw2jpg
#
# Usage: ./autosfw2jpg {roll_number} {date_prefix}
#

ROLL_NUMBER=$1
PICTURE_DATE=$2

# Configuration Settings - Update these to match your setup
BLOCK_DEVICE=/dev/sr0
DISK=/mnt/cdrom/$ROLL_NUMBER
DESTINATION=/home/user/pictures

# Temporary File Location
TEMPORARY_FOLDER=/tmp/temporary_folder_sfw2jpg
READY_FOLDER=$TEMPORARY_FOLDER/ready

folder_extension=""
file_extension=""
i=1
disk_folder=""

#
# Clear workspace
#
if [ -d $TEMPORARY_FOLDER ]; then
    rm -f -r $TEMPORARY_FOLDER
fi
if [ ! -d $TEMPORARY_FOLDER ]; then
    mkdir $TEMPORARY_FOLDER
    mkdir $TEMPORARY_FOLDER/ready
fi

#
# Copy images from disk
#
udisksctl mount -b $BLOCK_DEVICE

if [ -d "$DISK/$ROLL_NUMBER.hi" ]; then
    folder_extension="hi"
else
    folder_extension="rol"
fi

disk_folder=$DISK/$ROLL_NUMBER.$folder_extension

for x in $disk_folder/*; do
    ext="${x##*.}"
    if [[ $ext == sfw ]]; then
        file_extension=sfw
    elif [[ $ext == jpg ]]; then
        file_extension=jpg
    else
        echo file extension error
    fi
done

for x in $disk_folder/*; do
    let i++
    cp $x $TEMPORARY_FOLDER/"_image_$(printf '%02d' "${i}")".$file_extension
done

udisksctl unmount -b $BLOCK_DEVICE

#
# Convert if needed
#
for x in $TEMPORARY_FOLDER/*; do
    filename=${x##*/}
    final_file=$TEMPORARY_FOLDER/ready/$PICTURE_DATE${filename%.*}
    if [ -d $x ]; then
        break
    fi
    if [[ $file_extension == sfw ]]; then
        sfwjpg $x
        jpegtran -flip vertical ${x%.*}.jpg > $final_file.jpg
    elif [[ $file_extension == jpg ]]; then
        mv $x $final_file.jpg
    fi
done

#
# Move to Destination folder
#
for x in $READY_FOLDER/*; do
    mv $x $DESTINATION
done

rm -f -r $TEMPORARY_FOLDER

echo Complete
