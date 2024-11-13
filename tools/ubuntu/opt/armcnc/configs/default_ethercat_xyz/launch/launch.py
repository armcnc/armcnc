#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
import armcnc
sys.path.append("/usr/lib/python3/dist-packages")

def armcnc_start(sdk):
    sdk.utils.log.ignore("armcnc_start")
    while not sdk.quit_event.is_set():
        pass

def armcnc_message(sdk, message):
    sdk.utils.log.ignore("armcnc_message")

def armcnc_exit(sdk):
    sdk.utils.log.ignore("armcnc_exit")

if __name__ == "__main__":
    armcnc.Framework()

