#!/bin/bash

BIN=test

./$BIN &
PID=$(pidof $BIN)
ruby test.rb $PID
kill -9 $PID