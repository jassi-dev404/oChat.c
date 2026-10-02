#!/bin/bash

lsof -ti:6967 | xargs kill -9 2>/dev/null 
lsof -ti:6969 | xargs kill -9 2>/dev/null

cd C_stuff

curl -sLO https://raw.githubusercontent.com/DaveGamble/cJSON/master/cJSON.c 
curl -sLO https://raw.githubusercontent.com/DaveGamble/cJSON/master/cJSON.h

clang main.c cJSON.c -o server
./server

