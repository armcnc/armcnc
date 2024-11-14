#!/usr/bin/env python
# -*- coding: utf-8 -*-

import armcnc

def armcnc_start(sdk):
    sdk.utils.log.ignore("armcnc_start")
    while True:
        pass

def armcnc_message(sdk, message):
    pass

def armcnc_exit(sdk):
    sdk.utils.log.ignore("armcnc_exit")

if __name__ == "__main__":
    armcnc.Framework()
