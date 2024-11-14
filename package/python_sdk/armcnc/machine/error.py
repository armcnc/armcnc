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

import time
import threading
import linuxcnc

class Error:

    def __init__(self):
        self.linuxcnc = linuxcnc
        self.service = None
        self.machine = None
        self.stat = None
        self.api = self.linuxcnc.error_channel()
        self.task = threading.Thread(name="machine_error_task_work", target=self.machine_error_task_work)
        self.task_time_sleep = 0.05
        self.task.daemon = True
        self.task.start()
    
    def machine_error_task_work(self):
        while True:
            if self.machine is not None and self.machine.status:
                error = self.api.poll()
                if error:
                    kind, text = error
                    self.service.service_write({"command": "machine:error", "message": text, "data": kind})

            time.sleep(self.task_time_sleep)

