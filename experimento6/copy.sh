#!/bin/sh
echo "Copying table.c file..."
cp /usr/src/minix/servers/pm/table.c /mnt/table.c
echo "Copying callnr.h file..."
cp /usr/src/minix/include/minix/callnr.h /mnt/callnr.h
echo "Copying proto.h file..."
cp /usr/src/minix/servers/pm/proto.h /mnt/proto.h
echo "Copying mdc.c file..."
cp /usr/src/minix/servers/pm/mdc.c /mnt/mdc.c
echo "Copying pm/Makefile file..."
cp /usr/src/minix/servers/pm/Makefile /mnt/Makefile_pm
echo "Copying customlib.h file..."
cp /usr/src/include/customlib.h /mnt/customlib.h
echo "Copying include/Makefile file..."
cp /usr/src/include/Makefile /mnt/Makefile_include
echo "Copying ex1a.c file..."
cp /root/ex1a.c /mnt/ex1a.c
echo "Copying ex1b.c file..."
cp /root/ex1b.c /mnt/ex1b.c
echo "Copying ex2a.c file..."
cp /root/ex2a.c /mnt/ex2a.c
echo "Copying ex2b.c file..."
cp /root/ex2b.c /mnt/ex2b.c
echo "Finished copying everything!"
