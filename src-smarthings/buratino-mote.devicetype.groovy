/**
 *  Buratino Multi Sensor
 *
 *  Copyright 2014 Val Orekhov
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except
 *  in compliance with the License. You may obtain a copy of the License at:
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed
 *  on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License
 *  for the specific language governing permissions and limitations under the License.
 *
 */
metadata {
	definition (name: "Buratino Multi Sensor", namespace: "lab620.com", author: "Val Orekhov") {
		capability "Presence Sensor"
        capability "Refresh"
		capability "Sensor"
        capability "Configuration"

		fingerprint inClusters: "0000,0003,0406", profileId: "0104", outClusters: "0003"
	}

	// simulator metadata
	simulator {
		status "present": "catchall: 0104 0406 13 01 0140 00 CD4B 00 00 0000 01 01 0000001801"
		status "not present": "catchall: 0104 0406 13 01 0140 00 CD4B 00 00 0000 01 01 0000001800"
	}

	// UI tile definitions
	tiles {
		standardTile("presence", "device.presence", width: 2, height: 2, canChangeBackground: true) {
			state "present", labelIcon:"st.presence.tile.present", backgroundColor:"#53a7c0"
			state "not present", labelIcon:"st.presence.tile.not-present", backgroundColor:"#ffffff"
		}

		main "presence"
		details "presence"
	}
}



// parse events into attributes
def parse(String description) {
     def msg = zigbee.parse(description)
     log.trace "$msg"
     
     if (msg.clusterId == 0x0406 && msg.sourceEndpoint == 0x13){
     	def value = msg.data.last();
        log.trace "occupancy sensor value: $value"
     	return sendEvent(name: "presence", value:  value == 1 ? "present" : "not present")
     }
}


def refresh() {
	//log.debug "rattr 0x${device.deviceNetworkId} 0x13 0x0406 0"
    "st rattr 0x${device.deviceNetworkId} 0x13 0x0406 0"
}


def configure() {
	log.trace "Binding ZDO"
    [
        "zdo bind 0x${device.deviceNetworkId} 0x13 0x14 0x0406 {${device.zigbeeId}} {}"
    ]
}

