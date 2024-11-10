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

import os
import sys
import signal
from .config import Config
from .utils import Utils
from threading import Thread, Event
import launch as launch_file

class Framework:

    def __init__(self):
        signal.signal(signal.SIGINT, self.sigint_handler)
        signal.signal(signal.SIGTERM, self.sigint_handler)
        self.config = Config()
        self.utils = Utils()
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
                    self.config.set_path(env_var)
            getattr(launch_file, armcnc_start)(self)

    def sigint_handler(self, signum, frame):
        armcnc_exit = "armcnc_exit"
        if armcnc_exit in dir(launch_file):
            getattr(launch_file, armcnc_exit)(self)
        self.quit_event.set()
        sys.exit()