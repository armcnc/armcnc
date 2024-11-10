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

import json
import websocket
import threading

class Service:

    def __init__(self):
        self.socket = None
        self.status = False
        self.handle = None
        self.task = threading.Thread(name="service_task", target=self.service_task)
        self.task.daemon = True
        self.task.start()

    def service_task(self):
        websocket.enableTrace(False)
        self.socket = websocket.WebSocketApp(
            "ws://127.0.0.1:1081/message/index",
            on_message=self.service_message,
            on_error=self.service_error,
            on_close=self.service_close
        )
        self.socket.on_open = self.service_open
        self.socket.run_forever()

    def service_open(self, ws):
        self.status = True

    def service_write(self, message):
        if self.socket is not None and self.status:
            self.socket.send(json.dumps(message))

    def service_message(self, ws, message):
        if self.socket is not None and self.status:
            message_json = json.loads(message)
            if message_json["command"]:
                if self.handle:
                    self.handle(message_json)

    def service_error(self, ws, error):
        self.socket = None
        self.status = False

    def service_close(self, ws):
        self.socket = None
        self.status = False
