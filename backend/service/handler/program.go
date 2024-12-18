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

package handler

import (
	"github.com/armcnc/armcnc/backend/package/program"
	"github.com/armcnc/armcnc/backend/utils"
	"github.com/gin-gonic/gin"
)

type responseProgramRead struct {
	Lines []string `json:"lines"`
}

func ProgramRead(c *gin.Context) {

	returnData := responseProgramRead{}
	returnData.Lines = make([]string, 0)

	file := c.DefaultQuery("file", "")
	if file == "" {
		utils.Error(c, utils.EmptyData{})
		return
	}

	read := program.Get.ReadLine(file)
	returnData.Lines = read.Lines

	utils.Success(c, returnData)
}
