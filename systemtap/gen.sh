#!/bin/bash

rm -fr ./logs
mkdir ./logs

awk '/tid/ {print $5}' log | sort -u | tr -d ',' | fgrep -v ':' | xargs -I {} -n 1 -P 68  awk -v awk_tid=" {}" '$0 ~ awk_tid {print $0 > "./logs/"$5".log" }' log
