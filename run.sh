#!/bin/bash
clear
./Fat12Recovery test/test.img ./output1
echo -ne '\n'
./Fat12Recovery test/test2.img ./output2
echo -ne '\n'
./Fat12Recovery test/test3.img ./output3
