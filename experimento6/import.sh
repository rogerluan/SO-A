#!/bin/sh
echo "We're now importing all the files into our system."
echo "Importing table.c file..."
cp /mnt/table.c /usr/src/minix/servers/pm/table.c
echo "Importing callnr.h file..."
cp /mnt/callnr.h /usr/src/minix/include/minix/callnr.h
echo "Importing proto.h file..."
cp /mnt/proto.h /usr/src/minix/servers/pm/proto.h
echo "Importing mdc.c file..."
cp /mnt/mdc.c /usr/src/minix/servers/pm/mdc.c
echo "Importing pm/Makefile file..."
cp /mnt/Makefile_pm /usr/src/minix/servers/pm/Makefile
echo "Importing customlib.h file..."
cp /mnt/mylibrary.h /usr/src/include/mylibrary.h
echo "Importing include/Makefile file..."
cp /mnt/Makefile_include /usr/src/include/Makefile
echo "Importing ex1a.c file..."
cp /mnt/ex1a.c /root/ex1a.c
echo "Importing ex1b.c file..."
cp /mnt/ex1b.c /root/ex1b.c
echo "Importing ex2a.c file..."
cp /mnt/ex2a.c /root/ex2a.c
echo "Importing ex2b.c file..."
cp /mnt/ex2b.c /root/ex2b.c
echo "Finished importing everything!"
