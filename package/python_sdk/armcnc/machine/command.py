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

import linuxcnc

class Command:

    def __init__(self):
        self.linuxcnc = linuxcnc
        self.service = None
        self.machine = None
        self.stat = None
        self.api = self.linuxcnc.command()
    
    def on_start(self, line):
        if self.stat is not None:
            self.stat.poll()
            if self.stat.paused:
                self.on_restart()
                return False
            self.set_mode(linuxcnc.MODE_AUTO, 0)
            if self.stat.interp_state != linuxcnc.INTERP_IDLE:
                return False
            self.api.auto(linuxcnc.AUTO_RUN, int(line))

    def on_restart(self):
        if self.stat is not None:
            self.stat.poll()
            if not self.stat.paused:
                return False
            if self.stat.task_mode not in (linuxcnc.MODE_AUTO, linuxcnc.MODE_MDI):
                return False
            self.set_mode(linuxcnc.MODE_AUTO, 0.5, linuxcnc.MODE_MDI)
            self.api.auto(linuxcnc.AUTO_RESUME)
    
    def on_pause(self):
        if self.stat is not None:
            self.stat.poll()
            if self.stat.task_mode != linuxcnc.MODE_AUTO or self.stat.interp_state not in (linuxcnc.INTERP_READING, linuxcnc.INTERP_WAITING):
                return False
            self.api.auto(linuxcnc.AUTO_PAUSE)

    def on_stop(self):
        if self.stat is not None:
            self.set_mode(linuxcnc.MODE_AUTO, 0.5)
            self.api.abort()
            self.api.wait_complete()
            # 后续需要增加换刀信号的触发

    def check_mdi(self):
        if self.stat is not None:
            self.stat.poll()
            return not self.stat.estop and self.stat.enabled and self.stat.homed.count(1) == len(self.father.framework.machine.axes) and self.stat.interp_state == linuxcnc.INTERP_IDLE
        
    def set_mdi(self, command):
        if self.stat is not None:
            self.stat.poll()
            if self.check_mdi():
                self.set_mode(linuxcnc.MODE_MDI, 0.5)
                self.api.mdi(command)
    
    def get_mode(self):
        if self.stat is not None:
            self.stat.poll()
            return self.stat.task_mode
    
    def set_mode(self, m, t, *p):
        if self.stat is not None:
            self.stat.poll()
            if self.stat.task_mode == m or self.stat.task_mode in p:
                return True
            self.api.mode(m)
            if t == 0:
                self.api.wait_complete()
            else:
                self.api.wait_complete(t)
            self.stat.poll()
            return True

    def set_teleop_enable(self, value):
        if self.stat is not None:
            # 1:teleop, 0: joint
            self.stat.poll()
            self.api.teleop_enable(value)
            self.api.wait_complete()

    def set_motion_teleop(self, value):
        if self.stat is not None:
            self.api.teleop_enable(value)
            self.api.wait_complete(0.1)
            self.stat.poll()
    
    def set_teleop_enable_mode(self, value):
        if self.stat is not None:
            self.stat.poll()
            if self.stat.task_mode != linuxcnc.MODE_MANUAL:
                self.set_mode(linuxcnc.MODE_MANUAL, 1)
            if self.get_jog_mode():
                return
            self.set_motion_teleop(value)
            return True
    
    def get_jog_mode(self):
        if self.stat is not None:
            self.stat.poll()
            if self.stat.kinematics_type == linuxcnc.KINEMATICS_IDENTITY and self.is_homed():
                teleop_mode = 1
                mode = False
            elif self.stat.motion_mode == linuxcnc.TRAJ_MODE_FREE:
                teleop_mode = 0
                mode = True
            else:
                teleop_mode = 1
                mode = False
            if mode and self.stat.motion_mode != linuxcnc.TRAJ_MODE_FREE or not mode and self.stat.motion_mode != linuxcnc.TRAJ_MODE_TELEOP:
                self.set_teleop_enable(teleop_mode)
            return mode

    def jog_continuous(self, axis, speed, mode):
        if self.machine is not None and self.machine.task_state:
            if mode == "":
                mode = self.get_jog_mode()
            self.set_mode(linuxcnc.MODE_MANUAL, 0.5)
            self.api.jog(linuxcnc.JOG_CONTINUOUS, mode, int(axis), speed)

    def jog_increment(self, axis, speed, increment, mode):
        if self.machine is not None and self.machine.task_state:
            if mode == "":
                mode = self.get_jog_mode()
            increment = float(increment)
            self.set_mode(linuxcnc.MODE_MANUAL, 0.5)
            self.api.jog(linuxcnc.JOG_INCREMENT, mode, int(axis), speed, increment)
    
    def jog_stop(self, axis, mode):
        if mode == "":
            mode = self.get_jog_mode()
        self.api.jog(linuxcnc.JOG_STOP, mode, int(axis))
    
    def set_spindle_on(self, speed):
        self.set_spindle_speed(speed)
    
    def set_spindle_forward(self, speed):
        self.set_mode(linuxcnc.MODE_MANUAL, 0.5)
        self.api.spindle(linuxcnc.SPINDLE_FORWARD, speed)
    
    def set_spindle_reverse(self, speed):
        self.set_mode(linuxcnc.MODE_MANUAL, 0.5)
        self.api.spindle(linuxcnc.SPINDLE_REVERSE, speed)
    
    def set_spindle_faster(self):
        self.set_mode(linuxcnc.MODE_MANUAL, 0.5)
        self.api.spindle(0, linuxcnc.SPINDLE_INCREASE)
    
    def set_spindle_slower(self):
        self.set_mode(linuxcnc.MODE_MANUAL, 0.5)
        self.api.spindle(0, linuxcnc.SPINDLE_DECREASE)
    
    def set_spindle_off(self):
        self.set_mode(linuxcnc.MODE_MANUAL, 0.5)
        self.api.spindle(linuxcnc.SPINDLE_OFF)
    
    def set_spindle_speed(self, speed):
        if self.stat is not None:
            self.stat.poll()
            self.set_mode(linuxcnc.MODE_MANUAL, 0.5)
            if self.stat.spindle[0]["direction"] == 1 or self.stat.spindle[0]["direction"] == 0:
                self.api.spindle(linuxcnc.SPINDLE_FORWARD, speed)
            if self.stat.spindle[0]["direction"] == -1:
                self.api.spindle(linuxcnc.SPINDLE_REVERSE, speed)
    
    def is_spindle_running(self):
        if self.stat is not None:
            self.stat.poll()
            if self.stat.spindle[0]["enabled"]:
                return self.stat.spindle[0]["speed"]
            else:
                return 0
    
    def set_spindle_override(self, value):
        value = int(value) / 100.0
        self.api.spindleoverride(value)

    def set_max_velocity(self, value):
        value = float(value) / 60
        self.api.maxvel(value)

    def set_feed_rate(self, value):
        value = value / 100.0
        self.api.feedrate(value)

    def set_offset(self, data):
        command = data["name"]
        self.set_mdi(command)
        self.set_mode(linuxcnc.MODE_MANUAL, 0.5)

    def set_axis_offset(self, data):
        command = "G10 L20 " + data["name"] + " X" + data["x"] + " Y" + data["y"] + " Z" + data["z"]
        self.set_mdi(command)
        self.set_mode(linuxcnc.MODE_MANUAL, 0.5)
    
    def home_all(self):
        self.set_teleop_enable(0)
        self.api.home(-1)
        self.api.wait_complete()

    def home_axis(self, axis):
        self.set_teleop_enable(0)
        self.api.home(axis)
        self.api.wait_complete()

    def un_home_all(self):
        if self.machine is not None:
            for x in range(len(self.machine.axes) - 1, -1, -1):
                self.un_home_axis(x)

    def un_home_axis(self, axis):
        if self.stat is not None:
            self.stat.poll()
            if self.stat.task_mode != linuxcnc.MODE_MANUAL:
                self.set_mode(linuxcnc.MODE_MANUAL, 1)
            self.set_motion_teleop(0)
            self.api.unhome(axis)

    def is_homed(self):
        if self.machine is not None:
            axes = len(self.machine.axes)
            for i in range(0, axes):
                if self.machine.info["homed"][i] != 1:
                    return False
            return True
        else:
            return True
    
    def is_manual(self):
        if self.stat is not None:
            self.stat.poll()
            if self.stat.task_state != linuxcnc.STATE_ON:
                return False
            return self.stat.interp_state == linuxcnc.INTERP_IDLE or self.stat.task_mode == linuxcnc.MODE_MDI
    
    def override_limits(self):
        self.set_mode(linuxcnc.MODE_MANUAL, 0.5)
        self.api.override_limits()
        self.api.wait_complete(0.5)