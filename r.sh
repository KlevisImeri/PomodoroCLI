echo off
set -x
rm exe
clear
g++ $1.cpp -DLOCAL -o2 -std=c++20 -o exe -fdiagnostics-color=always -lsqlite3 2>&1 | head -n $2
./exe < in 2> debug.txt
rm exe
