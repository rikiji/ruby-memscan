#!/bin/bash

BIN=test

./$BIN &
PID=$(pidof $BIN)
ruby dev.rb $PID
kill -9 $PID