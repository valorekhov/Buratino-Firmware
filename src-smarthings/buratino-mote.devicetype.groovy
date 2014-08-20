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
		//capability "Presence Sensor"
		capability "Temperature Measurement"
		capability "Relative Humidity Measurement"        
        capability "Refresh"
		capability "Sensor"
        capability "Configuration"

		//fingerprint inClusters: "0000,0003,0406", profileId: "0104", outClusters: "0003"
        fingerprint inClusters: "0000,0003,0402,0405", profileId: "0104", outClusters: "0003" 
	}

	// simulator metadata
	simulator {
		status "present": "catchall: 0104 0406 13 01 0140 00 CD4B 00 00 0000 01 01 0000001801"
		status "not present": "catchall: 0104 0406 13 01 0140 00 CD4B 00 00 0000 01 01 0000001800"
        status "temp": "catchall: 0104 0402 13 01 0140 00 CD4B 00 00 0000 01 01 0000001800"
	}

	// UI tile definitions
	tiles {
    /*
		standardTile("presence", "device.presence", width: 2, height: 2, canChangeBackground: true) {
			state "present", labelIcon:"st.presence.tile.present", backgroundColor:"#53a7c0"
			state "not present", labelIcon:"st.presence.tile.not-present", backgroundColor:"#ffffff"
		}*/
        
		valueTile("temperature", "device.temperature", width: 2, height: 2) {
			state("temperature", label:'${currentValue}°',
				backgroundColors:[
					[value: 31, color: "#153591"],
					[value: 44, color: "#1e9cbb"],
					[value: 59, color: "#90d2a7"],
					[value: 74, color: "#44b621"],
					[value: 84, color: "#f1d801"],
					[value: 95, color: "#d04e00"],
					[value: 96, color: "#bc2323"]
				]
			)
		}
		valueTile("humidity", "device.humidity") {
			state "humidity", label:'${currentValue}%', unit:""
		}

		main(["temperature", "humidity"])
		details(["temperature", "humidity"])
	}
}



// parse events into attributes
def parse(String description) {
     //log.trace "$description"
     def msg = zigbee.parse(description)
     
     if (msg != null && msg.clusterId == 0x0406 && msg.sourceEndpoint == 0x13){
        log.trace "$msg"
     	def value = msg.data.last();
        log.trace "occupancy sensor value: $value"
     	return sendEvent(name: "presence", value:  value == 1 ? "present" : "not present")
     }
     
     def name = parseName(description)
	 def value = parseValue(description)     
	 def unit = name == "temperature" ? getTemperatureScale() : (name == "humidity" ? "%" : null)     
	 def result = createEvent(name: name, value: value, unit: unit)
	 log.trace "${result?.descriptionText}"
	 return result
}


def refresh() {
    //"st rattr 0x${device.deviceNetworkId} 0x13 0x0406 0"
    "st rattr 0x${device.deviceNetworkId} 0x13 0x0402 0"
    "st rattr 0x${device.deviceNetworkId} 0x13 0x0405 0"
}


def configure() {
	log.trace "Binding ZDO"
    [
        //"zdo bind 0x${device.deviceNetworkId} 0x13 0x14 0x0406 {${device.zigbeeId}} {}"
        "zdo bind 0x${device.deviceNetworkId} 0x13 0x14 0x0402 {${device.zigbeeId}} {}",
        "zdo bind 0x${device.deviceNetworkId} 0x13 0x14 0x0405 {${device.zigbeeId}} {}"
    ]
}

private String parseName(String description) {
	if (description?.startsWith("temperature: ")) {
		return "temperature"
	} else if (description?.startsWith("humidity: ")) {
		return "humidity"
	}
	null
}

private String parseValue(String description) {
	if (description?.startsWith("temperature: ")) {
		return zigbee.parseHATemperatureValue(description, "temperature: ", getTemperatureScale())
	} else if (description?.startsWith("humidity: ")) {
		def pct = (description - "humidity: " - "%").trim()
		if (pct.isNumber()) {
        	//ST hub performs RH Zigbee Cluster conversion not to ZCL spec.
            //accroding to ZCL, 10,000 (0x2710) value represents 100.00% humidity
            //yet ST stack reports this value as 39%
            //scale the value by * 2.5641 to correct
			return Math.round(new BigDecimal(pct)* 2.5641).toString()
		}
	}
	null
}