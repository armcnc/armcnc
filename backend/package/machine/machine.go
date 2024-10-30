// Copyright 2024 ARMCNC, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

package machine

import (
	"github.com/armcnc/armcnc/backend/utils/file"
	"github.com/armcnc/armcnc/backend/utils/ini"
	"path/filepath"
)

var Get = &Machine{}

type Machine struct {
	Path string `json:"path"`
}

func New(workspace string) *Machine {
	return &Machine{
		Path: filepath.Join(workspace, "/configs/"),
	}
}

func (machine *Machine) Info(path string) INI {
	data := INI{}
	exists, _ := fileUtils.PathExist(machine.Path + path + "/machine.ini")
	if exists {
		iniFile, err := iniUtils.Load(machine.Path + path + "/machine.ini")
		if err == nil {
			err = iniUtils.MapTo(iniFile, &data)
			data = machine.Default(data)
		}
	}
	return data
}

func (machine *Machine) Default(data INI) INI {
	if data.Emc.Machine == "" {
		data.Emc.Machine = "machine"
	}
	if data.Emc.Debug == "" {
		data.Emc.Debug = "0"
	}
	if data.Emc.Version == "" {
		data.Emc.Version = "1.1"
	}
	if data.Display.Display == "" {
		data.Display.Display = "halui"
	}
	if data.Display.CycleTime == "" {
		data.Display.CycleTime = "0.1000"
	}
	if data.Display.PositionOffset == "" {
		data.Display.PositionOffset = "RELATIVE"
	}
	if data.Display.PositionFeedback == "" {
		data.Display.PositionFeedback = "ACTUAL"
	}
	if data.Display.Arcdivision == "" {
		data.Display.Arcdivision = "64"
	}
	if data.Display.MaxFeedOverride == "" {
		data.Display.MaxFeedOverride = "1.5"
	}
	if data.Display.MinSpindleOverride == "" {
		data.Display.MinSpindleOverride = "0.3"
	}
	if data.Display.MaxSpindleOverride == "" {
		data.Display.MaxSpindleOverride = "1.5"
	}
	if data.Display.DefaultLinearVelocity == "" {
		data.Display.DefaultLinearVelocity = "30.000"
	}
	if data.Display.MinLinearVelocity == "" {
		data.Display.MinLinearVelocity = "0.000"
	}
	if data.Display.MaxLinearVelocity == "" {
		data.Display.MaxLinearVelocity = "100.000"
	}
	if data.Display.DefaultAngularVelocity == "" {
		data.Display.DefaultAngularVelocity = "13.333"
	}
	if data.Display.MinAngularVelocity == "" {
		data.Display.MinAngularVelocity = "0.000"
	}
	if data.Display.MaxAngularVelocity == "" {
		data.Display.MaxAngularVelocity = "16.667"
	}
	if data.Display.ProgramPrefix == "" {
		data.Display.ProgramPrefix = "../../files"
	}
	if data.Display.OpenFile == "" {
		data.Display.OpenFile = "../../programs/armcnc.ngc"
	}
	if data.Display.Increments == "" {
		data.Display.Increments = "10mm,5mm,1mm,.5mm,.1mm,.05mm,.01mm,.005mm"
	}
	if data.Python.PathAppend == "" {
		data.Python.PathAppend = "../../scripts/python"
	}
	if data.Python.Toplevel == "" {
		data.Python.Toplevel = "../../scripts/python/main.py"
	}
	if data.Filter.ProgramExtension == "" {
		data.Filter.ProgramExtension = ".py Python Script"
	}
	if data.Filter.Py == "" {
		data.Filter.Py = "python"
	}
	if data.Rs274ngc.Features == "" {
		data.Rs274ngc.Features = "30"
	}
	if data.Rs274ngc.SubroutinePath == "" {
		data.Rs274ngc.SubroutinePath = "../../scripts/ngc"
	}
	if data.Rs274ngc.ParameterFile == "" {
		data.Rs274ngc.ParameterFile = "machine.var"
	}
	if data.Emcmot.Emcmot == "" {
		data.Emcmot.Emcmot = "motmod"
	}
	if data.Emcmot.CommTimeout == "" {
		data.Emcmot.CommTimeout = "1.000"
	}
	if data.Emcmot.BasePeriod == "" {
		data.Emcmot.BasePeriod = "200000"
	}
	if data.Emcmot.ServoPeriod == "" {
		data.Emcmot.ServoPeriod = "1000000"
	}
	if data.Emcio.Emcio == "" {
		data.Emcio.Emcio = "io"
	}
	if data.Emcio.CycleTime == "" {
		data.Emcio.CycleTime = "0.100"
	}
	if data.Emcio.ToolTable == "" {
		data.Emcio.ToolTable = "machine.tbl"
	}
	if data.Task.Task == "" {
		data.Task.Task = "milltask"
	}
	if data.Task.CycleTime == "" {
		data.Task.CycleTime = "0.010"
	}
	if data.Hal.HalFile == "" {
		data.Hal.HalFile = "machine.hal"
	}
	if data.Traj.Spindles == "" {
		data.Traj.Spindles = "1"
	}
	if data.Traj.Coordinates == "" {
		data.Traj.Coordinates = "XYZ"
	}
	if data.Traj.LinearUnits == "" {
		data.Traj.LinearUnits = "mm"
	}
	if data.Traj.AngularUnits == "" {
		data.Traj.AngularUnits = "deg"
	}
	if data.Traj.PositionFile == "" {
		data.Traj.PositionFile = "machine.position"
	}
	if data.Spindle0.MinForwardVelocity == "" {
		data.Spindle0.MinForwardVelocity = "0"
	}
	if data.Spindle0.MaxForwardVelocity == "" {
		data.Spindle0.MaxForwardVelocity = "12000"
	}
	if data.Kins.Joints == "" {
		data.Kins.Joints = "3"
	}
	if data.Kins.Kinematics == "" {
		data.Kins.Kinematics = "trivkins coordinates=XYZ"
	}
	if data.AxisX.MaxVelocity == "" {
		data.AxisX.MaxVelocity = "0.000"
	}
	if data.AxisX.MaxAcceleration == "" {
		data.AxisX.MaxAcceleration = "0.000"
	}
	if data.AxisX.MinLimit == "" {
		data.AxisX.MinLimit = "0.000"
	}
	if data.AxisX.MaxLimit == "" {
		data.AxisX.MaxLimit = "0.000"
	}
	if data.Joint0.Type == "" {
		data.Joint0.Type = "LINEAR"
	}
	if data.Joint0.Home == "" {
		data.Joint0.Home = "0.000"
	}
	if data.Joint0.MaxVelocity == "" {
		data.Joint0.MaxVelocity = "0.000"
	}
	if data.Joint0.MaxAcceleration == "" {
		data.Joint0.MaxAcceleration = "0.000"
	}
	if data.Joint0.StepgenMaxaccel == "" {
		data.Joint0.StepgenMaxaccel = "0.000"
	}
	if data.Joint0.Scale == "" {
		data.Joint0.Scale = "1600"
	}
	if data.Joint0.Ferror == "" {
		data.Joint0.Ferror = "1.000"
	}
	if data.Joint0.MinLimit == "" {
		data.Joint0.MinLimit = "0.000"
	}
	if data.Joint0.MaxLimit == "" {
		data.Joint0.MaxLimit = "0.000"
	}
	if data.Joint0.HomeOffset == "" {
		data.Joint0.HomeOffset = "0.000"
	}
	if data.Joint0.HomeSearchVel == "" {
		data.Joint0.HomeSearchVel = "0.000"
	}
	if data.Joint0.HomeLarchVel == "" {
		data.Joint0.HomeLarchVel = "0.000"
	}
	if data.Joint0.HomeFinalVel == "" {
		data.Joint0.HomeFinalVel = "0.000"
	}
	if data.Joint0.VolatileHome == "" {
		data.Joint0.VolatileHome = "1"
	}
	if data.Joint0.HomeIgnoreLimits == "" {
		data.Joint0.HomeIgnoreLimits = "NO"
	}
	if data.Joint0.HomeUseIndex == "" {
		data.Joint0.HomeUseIndex = "NO"
	}
	if data.Joint0.HomeSequence == "" {
		data.Joint0.HomeSequence = "0"
	}
	if data.Joint0.Backlash == "" {
		data.Joint0.Backlash = "0.00"
	}
	if data.AxisY.MaxVelocity == "" {
		data.AxisY.MaxVelocity = "0.000"
	}
	if data.AxisY.MaxAcceleration == "" {
		data.AxisY.MaxAcceleration = "0.000"
	}
	if data.AxisY.MinLimit == "" {
		data.AxisY.MinLimit = "0.000"
	}
	if data.AxisY.MaxLimit == "" {
		data.AxisY.MaxLimit = "0.000"
	}
	if data.Joint1.Type == "" {
		data.Joint1.Type = "LINEAR"
	}
	if data.Joint1.Home == "" {
		data.Joint1.Home = "0.000"
	}
	if data.Joint1.MaxVelocity == "" {
		data.Joint1.MaxVelocity = "0.000"
	}
	if data.Joint1.MaxAcceleration == "" {
		data.Joint1.MaxAcceleration = "0.000"
	}
	if data.Joint1.StepgenMaxaccel == "" {
		data.Joint1.StepgenMaxaccel = "0.000"
	}
	if data.Joint1.Scale == "" {
		data.Joint1.Scale = "1600"
	}
	if data.Joint1.Ferror == "" {
		data.Joint1.Ferror = "1.000"
	}
	if data.Joint1.MinLimit == "" {
		data.Joint1.MinLimit = "0.000"
	}
	if data.Joint1.MaxLimit == "" {
		data.Joint1.MaxLimit = "0.000"
	}
	if data.Joint1.HomeOffset == "" {
		data.Joint1.HomeOffset = "0.000"
	}
	if data.Joint1.HomeSearchVel == "" {
		data.Joint1.HomeSearchVel = "0.000"
	}
	if data.Joint1.HomeLarchVel == "" {
		data.Joint1.HomeLarchVel = "0.000"
	}
	if data.Joint1.HomeFinalVel == "" {
		data.Joint1.HomeFinalVel = "0.000"
	}
	if data.Joint1.VolatileHome == "" {
		data.Joint1.VolatileHome = "1"
	}
	if data.Joint1.HomeIgnoreLimits == "" {
		data.Joint1.HomeIgnoreLimits = "NO"
	}
	if data.Joint1.HomeUseIndex == "" {
		data.Joint1.HomeUseIndex = "NO"
	}
	if data.Joint1.HomeSequence == "" {
		data.Joint1.HomeSequence = "0"
	}
	if data.Joint1.Backlash == "" {
		data.Joint1.Backlash = "0.00"
	}
	if data.AxisZ.MaxVelocity == "" {
		data.AxisZ.MaxVelocity = "0.000"
	}
	if data.AxisZ.MaxAcceleration == "" {
		data.AxisZ.MaxAcceleration = "0.000"
	}
	if data.AxisZ.MinLimit == "" {
		data.AxisZ.MinLimit = "0.000"
	}
	if data.AxisZ.MaxLimit == "" {
		data.AxisZ.MaxLimit = "0.000"
	}
	if data.Joint2.Type == "" {
		data.Joint2.Type = "LINEAR"
	}
	if data.Joint2.Home == "" {
		data.Joint2.Home = "0.000"
	}
	if data.Joint2.MaxVelocity == "" {
		data.Joint2.MaxVelocity = "0.000"
	}
	if data.Joint2.MaxAcceleration == "" {
		data.Joint2.MaxAcceleration = "0.000"
	}
	if data.Joint2.StepgenMaxaccel == "" {
		data.Joint2.StepgenMaxaccel = "0.000"
	}
	if data.Joint2.Scale == "" {
		data.Joint2.Scale = "1600"
	}
	if data.Joint2.Ferror == "" {
		data.Joint2.Ferror = "1.000"
	}
	if data.Joint2.MinLimit == "" {
		data.Joint2.MinLimit = "0.000"
	}
	if data.Joint2.MaxLimit == "" {
		data.Joint2.MaxLimit = "0.000"
	}
	if data.Joint2.HomeOffset == "" {
		data.Joint2.HomeOffset = "0.000"
	}
	if data.Joint2.HomeSearchVel == "" {
		data.Joint2.HomeSearchVel = "0.000"
	}
	if data.Joint2.HomeLarchVel == "" {
		data.Joint2.HomeLarchVel = "0.000"
	}
	if data.Joint2.HomeFinalVel == "" {
		data.Joint2.HomeFinalVel = "0.000"
	}
	if data.Joint2.VolatileHome == "" {
		data.Joint2.VolatileHome = "1"
	}
	if data.Joint2.HomeIgnoreLimits == "" {
		data.Joint2.HomeIgnoreLimits = "NO"
	}
	if data.Joint2.HomeUseIndex == "" {
		data.Joint2.HomeUseIndex = "NO"
	}
	if data.Joint2.HomeSequence == "" {
		data.Joint2.HomeSequence = "0"
	}
	if data.Joint2.Backlash == "" {
		data.Joint2.Backlash = "0.00"
	}
	if data.AxisA.MaxVelocity == "" {
		data.AxisA.MaxVelocity = "0.000"
	}
	if data.AxisA.MaxAcceleration == "" {
		data.AxisA.MaxAcceleration = "0.000"
	}
	if data.AxisA.MinLimit == "" {
		data.AxisA.MinLimit = "0.000"
	}
	if data.AxisA.MaxLimit == "" {
		data.AxisA.MaxLimit = "0.000"
	}
	if data.Joint3.Type == "" {
		data.Joint3.Type = "ANGULAR"
	}
	if data.Joint3.Home == "" {
		data.Joint3.Home = "0.000"
	}
	if data.Joint3.MaxVelocity == "" {
		data.Joint3.MaxVelocity = "0.000"
	}
	if data.Joint3.MaxAcceleration == "" {
		data.Joint3.MaxAcceleration = "0.000"
	}
	if data.Joint3.StepgenMaxaccel == "" {
		data.Joint3.StepgenMaxaccel = "0.000"
	}
	if data.Joint3.Scale == "" {
		data.Joint3.Scale = "1600"
	}
	if data.Joint3.Ferror == "" {
		data.Joint3.Ferror = "1.000"
	}
	if data.Joint3.MinLimit == "" {
		data.Joint3.MinLimit = "0.000"
	}
	if data.Joint3.MaxLimit == "" {
		data.Joint3.MaxLimit = "0.000"
	}
	if data.Joint3.HomeOffset == "" {
		data.Joint3.HomeOffset = "0.000"
	}
	if data.Joint3.HomeSearchVel == "" {
		data.Joint3.HomeSearchVel = "0.000"
	}
	if data.Joint3.HomeLarchVel == "" {
		data.Joint3.HomeLarchVel = "0.000"
	}
	if data.Joint3.HomeFinalVel == "" {
		data.Joint3.HomeFinalVel = "0.000"
	}
	if data.Joint3.VolatileHome == "" {
		data.Joint3.VolatileHome = "1"
	}
	if data.Joint3.HomeIgnoreLimits == "" {
		data.Joint3.HomeIgnoreLimits = "NO"
	}
	if data.Joint3.HomeUseIndex == "" {
		data.Joint3.HomeUseIndex = "NO"
	}
	if data.Joint3.HomeSequence == "" {
		data.Joint3.HomeSequence = "0"
	}
	if data.Joint3.Backlash == "" {
		data.Joint3.Backlash = "0.00"
	}
	if data.AxisB.MaxVelocity == "" {
		data.AxisB.MaxVelocity = "0.000"
	}
	if data.AxisB.MaxAcceleration == "" {
		data.AxisB.MaxAcceleration = "0.000"
	}
	if data.AxisB.MinLimit == "" {
		data.AxisB.MinLimit = "0.000"
	}
	if data.AxisB.MaxLimit == "" {
		data.AxisB.MaxLimit = "0.000"
	}
	if data.Joint4.Type == "" {
		data.Joint4.Type = "ANGULAR"
	}
	if data.Joint4.Home == "" {
		data.Joint4.Home = "0.000"
	}
	if data.Joint4.MaxVelocity == "" {
		data.Joint4.MaxVelocity = "0.000"
	}
	if data.Joint4.MaxAcceleration == "" {
		data.Joint4.MaxAcceleration = "0.000"
	}
	if data.Joint4.StepgenMaxaccel == "" {
		data.Joint4.StepgenMaxaccel = "0.000"
	}
	if data.Joint4.Scale == "" {
		data.Joint4.Scale = "1600"
	}
	if data.Joint4.Ferror == "" {
		data.Joint4.Ferror = "1.000"
	}
	if data.Joint4.MinLimit == "" {
		data.Joint4.MinLimit = "0.000"
	}
	if data.Joint4.MaxLimit == "" {
		data.Joint4.MaxLimit = "0.000"
	}
	if data.Joint4.HomeOffset == "" {
		data.Joint4.HomeOffset = "0.000"
	}
	if data.Joint4.HomeSearchVel == "" {
		data.Joint4.HomeSearchVel = "0.000"
	}
	if data.Joint4.HomeLarchVel == "" {
		data.Joint4.HomeLarchVel = "0.000"
	}
	if data.Joint4.HomeFinalVel == "" {
		data.Joint4.HomeFinalVel = "0.000"
	}
	if data.Joint4.VolatileHome == "" {
		data.Joint4.VolatileHome = "1"
	}
	if data.Joint4.HomeIgnoreLimits == "" {
		data.Joint4.HomeIgnoreLimits = "NO"
	}
	if data.Joint4.HomeUseIndex == "" {
		data.Joint4.HomeUseIndex = "NO"
	}
	if data.Joint4.HomeSequence == "" {
		data.Joint4.HomeSequence = "0"
	}
	if data.Joint4.Backlash == "" {
		data.Joint4.Backlash = "0.00"
	}
	if data.AxisC.MaxVelocity == "" {
		data.AxisC.MaxVelocity = "0.000"
	}
	if data.AxisC.MaxAcceleration == "" {
		data.AxisC.MaxAcceleration = "0.000"
	}
	if data.AxisC.MinLimit == "" {
		data.AxisC.MinLimit = "0.000"
	}
	if data.AxisC.MaxLimit == "" {
		data.AxisC.MaxLimit = "0.000"
	}
	if data.Joint5.Type == "" {
		data.Joint5.Type = "ANGULAR"
	}
	if data.Joint5.Home == "" {
		data.Joint5.Home = "0.000"
	}
	if data.Joint5.MaxVelocity == "" {
		data.Joint5.MaxVelocity = "0.000"
	}
	if data.Joint5.MaxAcceleration == "" {
		data.Joint5.MaxAcceleration = "0.000"
	}
	if data.Joint5.StepgenMaxaccel == "" {
		data.Joint5.StepgenMaxaccel = "0.000"
	}
	if data.Joint5.Scale == "" {
		data.Joint5.Scale = "1600"
	}
	if data.Joint5.Ferror == "" {
		data.Joint5.Ferror = "1.000"
	}
	if data.Joint5.MinLimit == "" {
		data.Joint5.MinLimit = "0.000"
	}
	if data.Joint5.MaxLimit == "" {
		data.Joint5.MaxLimit = "0.000"
	}
	if data.Joint5.HomeOffset == "" {
		data.Joint5.HomeOffset = "0.000"
	}
	if data.Joint5.HomeSearchVel == "" {
		data.Joint5.HomeSearchVel = "0.000"
	}
	if data.Joint5.HomeLarchVel == "" {
		data.Joint5.HomeLarchVel = "0.000"
	}
	if data.Joint5.HomeFinalVel == "" {
		data.Joint5.HomeFinalVel = "0.000"
	}
	if data.Joint5.VolatileHome == "" {
		data.Joint5.VolatileHome = "1"
	}
	if data.Joint5.HomeIgnoreLimits == "" {
		data.Joint5.HomeIgnoreLimits = "NO"
	}
	if data.Joint5.HomeUseIndex == "" {
		data.Joint5.HomeUseIndex = "NO"
	}
	if data.Joint5.HomeSequence == "" {
		data.Joint5.HomeSequence = "0"
	}
	if data.Joint5.Backlash == "" {
		data.Joint5.Backlash = "0.00"
	}
	return data
}
