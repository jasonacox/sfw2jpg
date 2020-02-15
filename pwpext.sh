#!/bin/sh

#
# pwpext
#
# Copyright (c) 1997-1999  Everett Lipman
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# The GNU General Public License is available at
#
#    http://www.fsf.org/copyleft/gpl.html
#
# Alternatively, you can write to the 
#
#    Free Software Foundation, Inc.
#    59 Temple Place - Suite 330
#    Boston, MA  02111-1307, USA
#

#
# pwpext - shell script to extract .jpg files from a .pwp file
#
# usage:  pwpext filename
#         where filename is the .pwp file
#
# NOTE: pwpjpg and jpegtran must be in your $PATH for this script to work.
#

exit_func()
{
echo ""
if [ "$1" = "1" ]
then
   echo "* pwpext aborted *"
else
   echo "*** pwpext finished ***"
fi
echo ""
exit $1
}

echo ""
echo "*** pwpext starting ***"
echo ""

if [ "$1" = "" ]
then
   echo "Please supply the name of your .pwp file:"
	echo ""
	echo "pwpext mypix.pwp"
   exit_func 1
fi

if [ -f $1 ]
then
	pwpname="$1"
else
	if [ -f "$1.pwp" ]
	then
		pwpname="$1.pwp"
	else
		if [ -f "$1.PWP" ]
		then
			pwpname="$1.PWP"
		else
			echo "$1: file not found"
			exit_func 1
		fi
	fi
fi

echo "Extracting pictures from $pwpname..."

#
# extract flipped jpeg files from .pwp file and save filenames in $picnames
#

picnames=`pwpjpg $pwpname`
echo "Done."
echo ""

echo "Flipping jpeg files..."
echo ""

#
# for each extracted file, flip image with jpegtran
#

for i in $picnames
do
	echo "flipping $i..."
	mv $i $i.tmp
	jpegtran -flip vertical < $i.tmp > $i
	rm -f $i.tmp
done

echo "" 
echo Done.

exit_func 0
