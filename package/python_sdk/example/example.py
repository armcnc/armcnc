#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os
import sys
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), "..")))
from armcnc import Framework

def on_start(sdk):
    sdk.utils.log.ignore("on_start")
    while not sdk.quit_event.is_set():
        pass

if __name__ == "__main__":
    framework = Framework()
    on_start(framework)
