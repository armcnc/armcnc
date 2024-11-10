# Copyright 2024 GEEKROS, Inc.
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

import os
import sys
import signal
from .utils import Utils
from .packages import Packages
from .interface import Interface
from threading import Thread, Event
import launch as launch_file

class Framework:

    def __init__(self):
        signal.signal(signal.SIGINT, self.sigint_handler)
        signal.signal(signal.SIGTERM, self.sigint_handler)
        self.utils = Utils()
        self.packages = Packages()
        self.interface = Interface()
        self.quit_event = Event()
        self.quit_thread = Thread(name="quit_task", target=self.quit_task)
        self.quit_thread.daemon = True
        self.quit_thread.start()

    def quit_task(self):
        armcnc_start = "armcnc_start"
        if armcnc_start in dir(launch_file):
            var_name = "MACHINE_PATH"
            if var_name in os.environ:
                env_var = os.environ[var_name]
                if env_var != "":
                    self.packages.machine.path = env_var
                    self.packages.service.handle = self.message_handle
                    self.interface.machine = self.packages.machine
                    self.interface.service = self.packages.service
            getattr(launch_file, armcnc_start)(self)
        # while not self.quit_event.is_set():
        #     pass
        # self.sigint_handler(False, False)

    def message_handle(self, message):
        armcnc_message = "armcnc_message"
        if armcnc_message in dir(launch_file):
            getattr(launch_file, armcnc_message)(self, message)

    def sigint_handler(self, signum, frame):
        armcnc_exit = "armcnc_exit"
        if armcnc_exit in dir(launch_file):
            getattr(launch_file, armcnc_exit)(self)
        self.quit_event.set()
        sys.exit()
