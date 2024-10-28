// Copyright 2024 GEEKROS, Inc.
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

package command

import (
	"github.com/gookit/color"
	"github.com/spf13/cobra"
	"log"
)

func Login() *cobra.Command {
	command := &cobra.Command{
		Use:     "login",
		Short:   "Login to Cloud Account",
		Long:    "Login to Cloud Account",
		Example: "armcnc login [appid] [appkey]",
		Run:     loginRun,
	}
	return command
}

func loginRun(cmd *cobra.Command, args []string) {
	if len(args) == 0 {
		log.Println(color.Yellow.Text("Your authorization key is invalid"))
		return
	}
}
