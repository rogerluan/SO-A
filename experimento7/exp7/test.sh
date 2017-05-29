#!/bin/sh
echo "Executing the tests..."

echo "Compiling the program..."
clang exp7.c -o exp7

echo "Cleaning the directory from previous files..."
rm novo
rm novo1
rm novo2
rm test_results.txt


echo "Running the 1st test:"
./exp7 write "aaabcdefghij" 12 seek 0 write "##" 2 >> test_results.txt

echo "Running the 2nd test:"
./exp7 open new "novo" write "aaabcdefghij" 12 seek 0 write "##" 2 close >> test_results.txt

echo "Running the 3rd test:"
./exp7 open new "novo1" write "aaabcdefghij" 12 seek 100 write "##" 2 close >> test_results.txt

echo "Running the 4th test:"
./exp7 open new "novo2" write "aaabcdefghij" 12 seek 100000 write "##" 2 seek 100001 write "@@@@@" 5 >> test_results.txt

echo "Running the 5th test:"
./exp7 open old "novo2" seek 50000 read 2 seek 100003 write >> test_results.txt

echo "Finished testing everything!"
