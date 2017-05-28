#!/bin/bash
rm -fr ./logs/*
for tid in `awk '/tid/ {print $5}' log | sort -u | tr -d ',' | fgrep -v ':' `; do echo $tid; awk -v awk_tid=$tid '$0 ~ awk_tid {print $0}' log > ./logs/$tid.log; done
