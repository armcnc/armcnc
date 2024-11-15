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
sys.path.append("/usr/lib/python3/dist-packages")
import signal
import linuxcnc
from .config import Config
from .utils import Utils
from .service import Service
from .machine import Machine
import launch as launch_file

class Framework:

    def __init__(self):
        signal.signal(signal.SIGINT, self.sigint_handler)
        signal.signal(signal.SIGTERM, self.sigint_handler)
        self.config = Config()
        self.utils = Utils()
        self.service = Service()
        self.machine = Machine()
        self.start()

    def start(self):
        armcnc_start = "armcnc_start"
        if armcnc_start in dir(launch_file):
            var_name = "MACHINE_PATH"
            if var_name in os.environ:
                env_var = os.environ[var_name]
                if env_var != "":
                    self.config.set_path(env_var)
                    self.utils.log.service = self.service
                    self.service.message_handle = self.server_message_handle
                    self.machine.config = self.config
                    self.machine.service = self.service
                    self.machine.start()
            getattr(launch_file, armcnc_start)(self)
        self.signal_handler(False, False)

    def server_message_handle(self, message):
        self.machine_message_handle(message)
        armcnc_message = "armcnc_message"
        if armcnc_message in dir(launch_file):
            getattr(launch_file, armcnc_message)(self, message)
    
    def machine_message_handle(self, message):
        if message and message["command"] and message["command"] != "":
            if message["command"] == "client:machine:program:open":
                if message["data"] != "":
                    try:
                        self.machine.command.api.program_open(self.config.get_workspace() + "/programs/" + message["data"])
                    except linuxcnc.error as e:
                        self.service.service_write({"command": "machine:program:open", "message": "", "data": {"status": False, "file": message["data"]}})
                        self.machine.command.set_mode(linuxcnc.MODE_MANUAL, 0.5)
            if message["command"] == "client:machine:estop":
                self.machine.stat.poll()
                if self.machine.stat.task_state == linuxcnc.STATE_ESTOP:
                    self.machine.command.api.state(linuxcnc.STATE_ESTOP_RESET)
                else: 
                    if self.machine.stat.task_state == linuxcnc.STATE_ESTOP_RESET or self.machine.stat.task_state == linuxcnc.STATE_ON or self.machine.stat.task_state == linuxcnc.STATE_OFF:
                        self.machine.command.api.state(linuxcnc.STATE_ESTOP)
                self.machine.command.api.wait_complete(0.5)
            if message["command"] == "client:machine:start":
                self.machine.command.on_start(message["data"]["line"])
            if message["command"] == "client:machine:pause":
                self.machine.command.on_pause()
            if message["command"] == "client:machine:stop":
                self.machine.command.on_stop()
            if message["command"] == "client:machine:device:override_limits":
                self.machine.command.override_limits()
            if message["command"] == "client:machine:device:home":
                if len(self.machine.axes) > 0:
                    if message["data"] == "all":
                        self.machine.command.home_all()
                    else:
                        value = int(message["data"])
                        self.machine.command.set_teleop_enable_mode(0)
                        self.machine.command.home_axis(value)
            if message["command"] == "client:machine:set:offset":
                if len(self.machine.axes) > 0:
                    self.machine.command.set_offset(message["data"])
            if message["command"] == "client:machine:relative:offset":
                if len(self.machine.axes) > 0:
                    self.machine.command.set_axis_offset(message["data"])
            if message["command"] == "client:machine:jog:start":
                axis = message["data"]["axis"]
                speed = message["data"]["speed"]
                jog_mode = self.machine.command.get_jog_mode()
                if jog_mode:
                    axis = self.machine.get_axes_num(axis)
                else:
                    axis = self.machine.get_axis_num(axis)
                speed = speed / 60
                increment = message["data"]["increment"]
                if increment == -1:
                    self.machine.command.jog_continuous(axis, speed, jog_mode)
                else:
                    self.machine.command.jog_increment(axis, speed, increment, jog_mode)
            if message["command"] == "client:machine:jog:stop":
                axis = message["data"]["axis"]
                jog_mode = self.machine.command.get_jog_mode()
                if jog_mode:
                    axis = self.machine.get_axes_num(axis)
                else:
                    axis = self.machine.get_axis_num(axis)
                self.machine.command.jog_stop(axis, jog_mode)
            if message["command"] == "client:machine:spindle":
                value = message["data"]["value"]
                speed = message["data"]["speed"]
                if value == "on":
                    self.machine.command.set_spindle_on(speed)
                if value == "forward":
                    self.machine.command.set_spindle_forward(speed)
                if value == "reverse":
                    self.machine.command.set_spindle_reverse(speed)
                if value == "faster":
                    self.machine.command.set_spindle_faster()
                if value == "slower":
                    self.machine.command.set_spindle_slower()
                if value == "off":
                    self.machine.command.set_spindle_off()
                if value == "speed":
                    self.machine.command.set_spindle_speed(speed)
            if message["command"] == "client:machine:spindle:override":
                value = message["data"]["value"]
                self.machine.command.set_spindle_override(value)
            if message["command"] == "client:machine:max:velocity":
                value = message["data"]["value"]
                self.machine.command.set_max_velocity(value)
            if message["command"] == "client:machine:feed:rate":
                value = message["data"]["value"]
                self.machine.command.set_feed_rate(value)
            if message["command"] == "client:machine:device:start":
                self.machine.stat.poll()
                if self.machine.stat.task_state == linuxcnc.STATE_ESTOP:
                    return False
                if self.machine.stat.task_state == linuxcnc.STATE_ON:
                    self.machine.command.api.state(linuxcnc.STATE_OFF)
                else:
                    if self.machine.stat.task_state == linuxcnc.STATE_OFF or self.machine.stat.task_state == linuxcnc.STATE_ESTOP_RESET:
                        self.machine.command.api.state(linuxcnc.STATE_ON)
                self.machine.command.api.wait_complete(0.5)
            if message["command"] == "client:machine:mdi":
                value = message["data"]["value"]
                if value != "":
                    self.machine.command.set_mdi(value)

    def sigint_handler(self, signum, frame):
        self.service.service_write({"command": "client:restart", "message": "", "data": False})
        armcnc_exit = "armcnc_exit"
        if armcnc_exit in dir(launch_file):
            getattr(launch_file, armcnc_exit)(self)
        self.machine.status = False
        sys.exit()