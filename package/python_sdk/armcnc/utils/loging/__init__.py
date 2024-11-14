# Copyright 2024 ARMCNC, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import sys
import logging
import colorlog

class Loging:

    def __init__(self):
        self.service = None
        self.logger = logging.getLogger(None)
        self.logger.handlers = []
        self.logger.setLevel(logging.DEBUG)
        console_fmt = "%(log_color)s%(asctime)s %(levelname)s: %(message)s"
        color_config = {
            "DEBUG": "cyan",
            "INFO": "green",
            "WARNING": "yellow",
            "ERROR": "red",
            "CRITICAL": "purple",
        }
        console_formatter = colorlog.ColoredFormatter(fmt=console_fmt, log_colors=color_config)
        console_handler = logging.StreamHandler(sys.stdout)
        console_handler.setFormatter(console_formatter)
        console_handler.setStream(sys.stdout) 
        self.logger.addHandler(console_handler)

    def ignore(self, log):
        self.logger.debug(log)
        if self.service is not None:
            self.service.service_write({"command": "log:ignore", "message": log, "data": False})

    def debug(self, log):
        self.logger.debug(log)
        if self.service is not None:
            self.service.service_write({"command": "log:debug", "message": log, "data": False})

    def info(self, log):
        self.logger.info(log)
        if self.service is not None:
            self.service.service_write({"command": "log:info", "message": log, "data": False})

    def warning(self, log):
        self.logger.warning(log)
        if self.service is not None:
            self.service.service_write({"command": "log:warning", "message": log, "data": False})

    def error(self, log):
        self.logger.error(log)
        if self.service is not None:
            self.service.service_write({"command": "log:error", "message": log, "data": False})
