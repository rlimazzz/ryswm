#!/bin/sh

set -e

Xephyr :2 -screen 1280x720 -ac -br -noreset &
sleep 1

DISPLAY=:2 ./xinitrc