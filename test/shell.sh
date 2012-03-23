#!/bin/bash

BIN=test

./$BIN &
PID=$(pidof $BIN)
ruby shell.rb $PID
kill -9 $PID