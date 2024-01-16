# autosfw2jpg

A bash script which converts and copies all `.sfw` image files from a seattle film works CDROM.

Author: Dean Gadberry https://github.com/DeanGadberry

## Setup

Do not run this script without first editing the VARIABLES in the first lines. 

* BLOCK_DEVICE - Device for CDROM (e.g. /dev/sr0)
* DISK - Mount location of CDROM (e.g. /mnt/cdrom)
* DESTINATION - Destination path name (e.g. /home/user/pictures)

## Arguments

```bash
./autosfw2jpg {roll_number} {date_prefix}
```

+ $1 ROLL_NUMBER 
+ $2 DATE (this will prepend file names)
