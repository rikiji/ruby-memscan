#!/bin/bash

BIN=test

./$BIN &
PID=$(pidof $BIN)
ruby dumpmap.rb $PID
kill -9 $PID