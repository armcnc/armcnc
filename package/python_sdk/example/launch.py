#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os
import sys
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), "..")))
from armcnc import Framework

def armcnc_start(sdk):
    sdk.utils.log.ignore("armcnc_start")
    while not sdk.quit_event.is_set():
        pass

def armcnc_message(sdk, message):
    sdk.utils.log.ignore("armcnc_message")

def armcnc_exit(sdk):
    sdk.utils.log.ignore("armcnc_exit")

if __name__ == "__main__":
    framework = Framework()
