#!/bin/sh
echo "We're now exporting all the files to our shared folder."
echo "Exporting table.c file..."
cp /usr/src/minix/servers/pm/table.c /mnt/table.c
echo "Exporting callnr.h file..."
cp /usr/src/minix/include/minix/callnr.h /mnt/callnr.h
echo "Exporting proto.h file..."
cp /usr/src/minix/servers/pm/proto.h /mnt/proto.h
echo "Exporting mdc.c file..."
cp /usr/src/minix/servers/pm/mdc.c /mnt/mdc.c
echo "Exporting pm/Makefile file..."
cp /usr/src/minix/servers/pm/Makefile /mnt/Makefile_pm
echo "Exporting customlib.h file..."
cp /usr/src/include/mylibrary.h /mnt/mylibrary.h
echo "Exporting include/Makefile file..."
cp /usr/src/include/Makefile /mnt/Makefile_include
echo "Exporting ex1a.c file..."
cp /root/ex1a.c /mnt/ex1a.c
echo "Exporting ex1b.c file..."
cp /root/ex1b.c /mnt/ex1b.c
echo "Exporting ex2a.c file..."
cp /root/ex2a.c /mnt/ex2a.c
echo "Exporting ex2b.c file..."
cp /root/ex2b.c /mnt/ex2b.c
echo "Finished exporting everything!"
