#!/bin/bash

sudo ./stap -g  -v -r /home/gromero/git/linux ./$1.d  | tee log
