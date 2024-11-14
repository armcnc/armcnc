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
import hal
import time
import copy
import subprocess
import threading
import configparser
import linuxcnc
from .command import Command
from .error import Error

class Machine:

    def __init__(self):
        self.config = None
        self.service = None
        self.linuxcnc = linuxcnc
        self.hal = hal
        self.stat = self.linuxcnc.stat()
        self.command = Command()
        self.error = Error()
        self.info = None
        self.axes = []
        self.axes_tmp = ""
        self.axis_tmp = ""
        self.data = {"index": 0, "position": {}, "velocity": {}, "g_offset": {}, "g5x_offset": {}, "g92_offset": {}, "dtg_offset": {}, "tool": {}, "options": []}
        self.status = False
        self.task_state = False
        self.task = threading.Thread(name="machine_task_work", target=self.machine_task_work)
        self.task_time_sleep = 0.05
        self.task.daemon = True
        self.task.start()
    
    def start(self):
        linuxcnc_pid = subprocess.Popen(["pidof", "-x", "linuxcnc"], stdout=subprocess.PIPE)
        linuxcnc_pid_result = linuxcnc_pid.communicate()[0]
        if len(linuxcnc_pid_result) == 0:
            self.service.service_write({"command": "machine:error", "message": "", "data": False})
            sys.exit()
        else:
            self.command.machine = self
            self.command.service = self.service
            self.command.stat = self.stat
            self.error.machine = self
            self.error.service = self.service
            self.error.stat = self.stat
            self.status = True
            self.service.service_write({"command": "machine:restart", "message": "", "data": True})

    def machine_task_work(self):
        while True:
            if self.status and self.config is not None and self.service is not None:
                try:
                    self.stat.poll()
                except linuxcnc.error as detail:
                    self.service.service_write({"command": "machine:error", "message": detail, "data": False})

                self.info = {}

                for item in dir(self.stat):
                    if not item.startswith("_") and not callable(getattr(self.stat, item)):
                        self.info[item] = getattr(self.stat, item)
                
                if self.info["ini_filename"]:
                    inifile = linuxcnc.ini(self.info["ini_filename"])
                    info_data = {
                        "user": self.config.user,
                        "workspace": self.config.workspace,
                        "machine_path": self.config.path,
                        "control": int(self.get_user_config_value("BASE", "CONTROL") or 0),
                        "increments": [value.replace("mm", "") for value in inifile.find("DISPLAY", "INCREMENTS").split(",")],
                        "axes": list(inifile.find("TRAJ", "COORDINATES")) or [],
                        "data": self.set_data(self.info["g5x_index"]),
                        "linear_units": inifile.find("TRAJ", "LINEAR_UNITS") or "mm",
                        "angular_units": inifile.find("TRAJ", "ANGULAR_UNITS") or "degree",
                        "estop": self.info["estop"],
                        "paused": self.info["paused"],
                        "enabled": self.info["enabled"],
                        "state": self.info["state"],
                        "interp_state": self.info["interp_state"],
                        "task_state": self.info["task_state"],
                        "homed": self.info["homed"],
                        "is_homed": self.command.is_homed(),
                        "motion_line": self.info["motion_line"],
                        "current_velocity": float(self.info["current_vel"]),
                        "spindle": {
                            "enabled": self.info["spindle"][0]["enabled"],
                            "direction": self.info["spindle"][0]["direction"],
                            "speed": self.info["spindle"][0]["speed"],
                            "default_speed": int(self.get_user_config_value("SPINDLE", "DEFAULT_SPINDLE_SPEED") or 1200),
                            "min_velocity": int(inifile.find("SPINDLE_0", "MIN_FORWARD_VELOCITY") or 0),
                            "max_velocity": int(inifile.find("SPINDLE_0", "MAX_FORWARD_VELOCITY") or 24000),
                            "min_override": float(inifile.find("DISPLAY", "MIN_SPINDLE_OVERRIDE")),
                            "max_override": float(inifile.find("DISPLAY", "MAX_SPINDLE_OVERRIDE")),
                            "override": self.info["spindle"][0]["override"],
                            "override_enabled": self.info["spindle"][0]["override_enabled"]
                        },
                        "feed": {
                            "max_override": float(inifile.find("DISPLAY", "MAX_FEED_OVERRIDE")),
                            "override": float(self.info["feedrate"])
                        },
                        "max_velocity": float(self.info["max_velocity"]),
                        "max_linear_velocity": float(inifile.find("DISPLAY", "MAX_LINEAR_VELOCITY")),
                        "default_linear_velocity": float(inifile.find("DISPLAY", "DEFAULT_LINEAR_VELOCITY")),
                        "max_angular_velocity": float(inifile.find("DISPLAY", "MAX_ANGULAR_VELOCITY")),
                        "default_angular_velocity": float(inifile.find("DISPLAY", "DEFAULT_ANGULAR_VELOCITY"))
                    }

                    self.axes = info_data["axes"]

                    axis_tmp = copy.copy(self.info["axis"])
                    g_offset_tmp = copy.copy(self.info["actual_position"])
                    g5x_offset_tmp = copy.copy(self.info["g5x_offset"])
                    g92_offset_tmp = copy.copy(self.info["g92_offset"])
                    dtg_offset_tmp = copy.copy(self.info["dtg"])
                    for i in range(0, len(info_data["axes"])):
                        actual_position = self.info["actual_position"]
                        axis_name = info_data["axes"][i]
                        axis_num = self.get_axis_num(axis_name)
                        axis = actual_position[axis_num] - g5x_offset_tmp[axis_num] - self.info["tool_offset"][axis_num]
                        #if self.info["joint"][axis_num]["homing"] == 1:
                        #    axis = self.info["joint_actual_position"][axis_num] - g5x_offset_tmp[axis_num] - self.info["tool_offset"][axis_num]
                        axis -= g92_offset_tmp[axis_num]
                        axis = "{:.3f}".format(axis)
                        info_data["data"]["position"][i] = axis
                        g_offset = g_offset_tmp[axis_num]
                        g_offset = "{:.3f}".format(g_offset)
                        info_data["data"]["g_offset"][i] = g_offset
                        g5x_offset = g5x_offset_tmp[axis_num]
                        g5x_offset = "{:.3f}".format(g5x_offset)
                        info_data["data"]["g5x_offset"][i] = g5x_offset
                        g92_offset = g92_offset_tmp[axis_num]
                        g92_offset = "{:.3f}".format(g92_offset)
                        info_data["data"]["g92_offset"][i] = g92_offset
                        dtg_offset = dtg_offset_tmp[axis_num]
                        dtg_offset = "{:.3f}".format(dtg_offset)
                        info_data["data"]["dtg_offset"][i] = dtg_offset
                        info_data["data"]["velocity"][i] = axis_tmp[axis_num]["velocity"]
                    
                    info_data["data"]["tool"] = {
                        "id": self.info["tool_table"][0].id,
                        "offset": self.info["tool_table"][0].zoffset,
                        "diameter": self.info["tool_table"][0].diameter,
                        "item": self.info["tool_table"][0]
                    }

                    if info_data["task_state"] == 4:
                        self.task_state = True
                    else:
                        self.task_state = False
                    
                    info_data["time"] = int(time.time() * 1000)
                    
                    self.info["format_data"] = info_data

                    self.service.service_write({"command": "machine:data", "message": "", "data": self.info})
            time.sleep(self.task_time_sleep)

    def set_data(self, index):
        self.data["index"] = index
        if len(self.data["options"]) == 0:
            for key, val in enumerate(range(9)):
                if key > 5:
                    self.data["options"].append({"label": "P" + str(key + 1) + " G59." + str((key - 6) + 1), "p_name": "P" + str(key + 1), "value": key + 1, "name": "G59." + str((key - 6) + 1)})
                else:
                    self.data["options"].append({"label": "P" + str(key + 1) + " G5" + str(key + 4), "p_name": "P" + str(key + 1), "value": key + 1, "name": "G5" + str(key + 4)})
        return self.data

    def get_axes_num(self, axes):
        self.axes_tmp = ''.join(self.axes)
        num = self.axes_tmp.find(axes.upper())
        return num

    def get_axis_num(self, axis):
        self.axis_tmp = "XYZABCUVW"
        num = self.axis_tmp.find(axis.upper())
        return num

    def get_axis_name(self, num):
        self.axis_tmp = "XYZABCUVW"
        name = self.axis_tmp[num]
        return name

    def get_user_config_value(self, father, value):
        config = configparser.ConfigParser()
        config.read(f"{self.config.workspace}/configs/{self.config.path}/machine.user")
        return config[father][value].strip()
    
    def get_user_config_array(self, father):
        config = configparser.ConfigParser()
        config.read(f"{self.config.workspace}/configs/{self.config.path}/machine.user")
        items = config.items(father)
        return items

    def get_user_config_items(self, father):
        configs = {}
        config = configparser.ConfigParser()
        config.read(f"{self.config.workspace}/configs/{self.config.path}/machine.user")
        items = config.items(father)
        for key, val in items:
            key = key.upper()
            configs[key] = val.strip()
        return configs