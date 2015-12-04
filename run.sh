#!/bin/bash
clear
./notjustcats test/test.img ./output1
echo -ne '\n'
./notjustcats test/test2.img ./output2
echo -ne '\n'
./notjustcats test/test3.img ./output3
