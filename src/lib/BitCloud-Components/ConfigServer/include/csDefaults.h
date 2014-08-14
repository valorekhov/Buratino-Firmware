/**************************************************************************//**
  \file csDefaults.h

  \brief
  Configuration Server parameters default values.
  The contents of this file must not be changed by a user. Any default value can be redefined
  in the application \c configuration.h file.

  Most of the parameters descriptions include the following standard remarks:

  \li <b>Value range</b> notes describe possible values that can be taken by a parameter;
  \li <b>C-type</b> stands for the type in code that can be used to hold a value of a parameter;
  \li <b>Can be set</b> indicates when a parameter can be changed: <i>at compile time only</i>, <i>at any time before network start</i>, or <i>at any time</i>;

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2014 , Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    19.10.10 A. Razinkov - Created.
******************************************************************************/

#ifndef _CSDEFAULTS_H
#define _CSDEFAULTS_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include <stackVersion.h>

/* MISRA killing for IAR compiler */
#ifdef  __IAR_SYSTEMS_ICC__
  #ifndef _SYSTEM_BUILD
    #pragma system_include
  #endif
#endif

/******************************************************************************
                    Definitions section
******************************************************************************/
#ifdef CS_APS_DATA_REQ_BUFFER_SIZE
 #error CS_APS_DATA_REQ_BUFFER_SIZE definition is renamed to CS_APS_DATA_REQ_BUFFERS_AMOUNT - please, use the new name.
#endif
#ifdef CS_APS_ACK_FRAME_BUFFER_SIZE
 #error CS_APS_ACK_FRAME_BUFFER_SIZE definition is renamed to CS_APS_ACK_FRAME_BUFFERS_AMOUNT - please, use the new name.
#endif

//! \brief The number of buffers for ZDP command requests on the ZDO layer
/*!
The parameter specifies the number of buffers that are allocated by ZDO
to store outgoing ZDP command requests parameters. The parameter puts an upper bound
to the number of ZDP requests that can be processed by ZDo simultaneously.

<b>C-type:</b> uint8_t \n
<b>Can be set:</b> at compile time only \n
*/
#ifndef CS_ZDO_COMMANDS_AMOUNT
  #define CS_ZDO_COMMANDS_AMOUNT       3
#endif

//BOOST mode is enabled automatically only for 11, 10, 9, 8, 7, 6 dBm(s) all other power valuses do not use BOOST mode.
//! \brief RF transmission power
/*!
The parameter specifies the TX power of the transceiver device, is measured in dBm(s).
After the node has entered the network the value can only be changed via
the ZDO_SetTxPowerReq() function.

<b>Value range:</b> depends on the hardware
<b>C-type:</b> int8_t \n
<b>Can be set:</b> at any time \n
*/
#ifndef CS_RF_TX_POWER
/* It was TX_PWR_3_0DBM. */
#define CS_RF_TX_POWER                        0
#endif

//! \brief Determines the device extended address
/*! 64-bit Unique Identifier (UID). If this value is 0 stack will try to read
hardware UID from external UID or EEPROM chip at startup. Location of hardware
UID is platform dependend and it may not be available on all platforms. If the latter
case then UID value must be provided by user via this parameter. This parameter must
be unique for each device in a network.

<b>Value range:</b> any 64-bit value except for broadcast extended addresses
(see apsCommon.h) \n
<b>C-type:</b> ExtAddr_t \n
<b>Can be set:</b> at any time before network start \n
*/
#ifndef CS_UID
#define CS_UID                                0x0000000000000000LL
#endif

//! \brief The maximum duration in ms of frame transmission
/*!
The parameter is used in some internal calculations as the maximum duration
of transmitting a frame to the air. The default value should not be changed by the user.

<b>C-type:</b> uint8_t \n
<b>Can be set:</b> at any time before network start \n
*/
#ifndef CS_MAX_FRAME_TRANSMISSION_TIME
#define CS_MAX_FRAME_TRANSMISSION_TIME         5
#endif

//! \brief Size of MAC RX buffer to store data frames
/*!
The parameter specifies a size of the buffer used by the MAC component for
data frames. The default value must not be changed by the user.

<b>Value range:</b> the value must be greater than 132 \n
<b>C-type:</b> uint16_t \n
<b>Can be set:</b> at compile time only \n
*/
#ifndef CS_MAC_FRAME_RX_BUFFER_SIZE
#define CS_MAC_FRAME_RX_BUFFER_SIZE            133
#endif

#ifdef _FFD_

//! \brief Size of MAC RX buffer to store data request frames
/*!
The parameter specifies a size of the buffer used by the MAC component for
data request frames. The default value must not be changed by the user.

<b>Value range:</b> the value must be greater than 27 \n
<b>C-type:</b> uint16_t \n
<b>Can be set:</b> at compile time only \n
*/
#ifndef CS_MAC_FRAME_DATA_REQ_BUFFER_SIZE
#define CS_MAC_FRAME_DATA_REQ_BUFFER_SIZE            108
#endif

#ifdef _PENDING_EMPTY_DATA_FRAME_
/*!
The paramemter is taken into account on FFD devices (routers and coordinators) only.

If the parameter equals \c true, then the FFD device will send data frame with 
empty payload and without acknowledgement request to his child in case the child's poll 
request has been acknowledged with pending data bit set and no real corresponding 
transaction were found in the transaction queue.

For more details see 7.5.6.3 "Extracting pending data from a coordinator" in IEEE 802.15.4-2006.

<b>Value range:</b> \c true or \c false \n
<b>C-type:</b> bool \n
<b>Can be set:</b> at any time \n
*/
#ifndef CS_MAC_SEND_EMPTY_DATA_FRAME
#define CS_MAC_SEND_EMPTY_DATA_FRAME          true
#endif /*CS_MAC_SEND_EMPTY_DATA_FRAME*/
#endif /*_FFD_*/
#endif /*_PENDING_EMPTY_DATA_FRAME_*/

#if defined(AT86RF212) || defined(CUSTOMRF3)
#ifndef CS_LBT_MODE
#define CS_LBT_MODE false
#endif
#endif

#ifdef _MAC2_
//! \brief MAC transaction persistence time
/*!
The parameter determines how long a frame received by the parent of a sleeping
end device is stored. If the end device does not polls for data during this time, then
the frame is dropped.

For more details see MAC PIB attributes, Table 86 in IEEE 802.15.4-2006.

<b>C-type:</b> uint32_t \n
<b>Can be set:</b> at any time \n
*/
// (by default in MAC = 7680L)
#ifndef CS_MAC_TRANSACTION_TIME
#define CS_MAC_TRANSACTION_TIME               7680L
#endif

#else  //_MAC2_

//! \brief Specifies receiver state (enabled or disabled) during inactive period for an end device
/*!
The parameter is taken into account on end devices only. Other devices behave as
if the parameter equals \c true.

If on an end device the parameter equals \c true,
then the end device can receive data at any time, radio is always on, and
its parent, which is informed about the parameter's value during association,
sends data to the child immediately  upon receiving a frame for the child.

Switching the parameter to \c false on an end devices turns on indirect delivery:
the end device's parent suspends data delivery to the child until it receives
a polling request from the child; on the end device radio is only on when data is
being sent.

<b>Value range:</b> \c true or \c false \n
<b>C-type:</b> bool \n
<b>Can be set:</b> at any time before network start \n
*/
#ifndef CS_RX_ON_WHEN_IDLE
#define CS_RX_ON_WHEN_IDLE                    false
#endif

// \cond internal
//! \brief Protocol version identifier. Could not be changed by user.
#ifndef CS_PROTOCOL_VERSION
#define CS_PROTOCOL_VERSION                   0x02
#endif
//! \brief Stack profile identifier (Zigbee PRO profile is equal to 2). Could not be changed by user.
#ifndef CS_STACK_PROFILE
#define CS_STACK_PROFILE                      0x02
#endif
// \endcond
//! \brief Default value for the type of a device
/*!
ZigBee device type determines network behavior of a given device and functions it
can perform. To give a brief overview, each networks contains exacty one coordinator and
an arbirtary number of routers and end devices; an end device does not have children, data
 is passed through the parent, that is, a router or the coordinator.

<b>Value range:</b> \n
::DEVICE_TYPE_COORDINATOR (0) - the coordinator\n
::DEVICE_TYPE_ROUTER (1) - a router\n
::DEVICE_TYPE_END_DEVICE (2) - an end device

<b>C-type:</b> DeviceType_t \n
<b>Can be set:</b> at any time before network start \n
*/
#ifndef CS_DEVICE_TYPE
#define CS_DEVICE_TYPE                        DEVICE_TYPE_ROUTER
#endif

//! \brief The size of the neighbor table
/*!
The parameter determines the size of the neighbor table which is used to store
beacon responses from nearby devices. The parameter puts an upper bound
over the amount of child devices possible for the node.

<b>Value range:</b>  at minimum 1, the maximum value is limited to the available memory \n
<b>C-type:</b> uint8_t \n
<b>Can be set:</b> at compile time only \n
*/
#ifndef CS_NEIB_TABLE_SIZE
  #define CS_NEIB_TABLE_SIZE                  7
#elif CS_NEIB_TABLE_SIZE == 0
  #undef  CS_NEIB_TABLE_SIZE
  #define CS_NEIB_TABLE_SIZE                  1
  #warning  CS_NEIB_TABLE_SIZE was set to 1
#endif

//! \brief The maximum number of direct children that a given device (the coordinator or a router) can have
/*!
The parameter is only enabled for routers and the coordinator. An end device can not
have children. If an actual number of children reaches a parameter's value, the node
will have not been able to accept any more children joining the network. The parameter can be
set to 0 on a router thus preventing it from accepting any children and can be help form a
desired network topology. For example, if the parameter is set to 0 on all routers, then the
coordinator will be the only device that can have children and the network will have star topology.

<b>Value range:</b>  from 0 to ::CS_NEIB_TABLE_SIZE \n
<b>C-type:</b> uint8_t \n
<b>Can be set:</b>  at compile time only \n
*/
#ifndef CS_MAX_CHILDREN_AMOUNT
#define CS_MAX_CHILDREN_AMOUNT                6
#endif
//! \brief The maximum number of routers among the direct children of the device
/*!
The parameter determines how many routers the device can have as children. Note that the maximum number of
end devices is equal to ::CS_MAX_CHILDREN_AMOUNT - ::CS_MAX_CHILDREN_ROUTER_AMOUNT.

<b>Value range:</b>  from 0 to ::CS_MAX_CHILDREN_AMOUNT \n
<b>C-type:</b> uint8_t \n
<b>Can be set:</b> at compile time only \n
 */
#ifndef CS_MAX_CHILDREN_ROUTER_AMOUNT
#define CS_MAX_CHILDREN_ROUTER_AMOUNT         2
#endif
//! \brief The maximum depth of a network
/*!
The parameter determines the maximum depth of a network tree formed by
child-parent relationships between nodes.

While joining the network the node receives beacon responses from potential
parents containing their actual network depth and declines those which show values not
less than the maximum network depth on the  joining device. A potential parent will also reject a
beacon from the joining device and will not sent a response if the joining device shows the network
depth greater than it is allowed on the potential parent. This logic is enabled if the parameter value
is not greater  than 15. If its value is greater than 15, then device does not perform any checkings of
the network depth, neither when joining a network nor when accepting other nodes as children.
This allows forming long chains of devices across considerable distances.

The stack also uses the parameter to calculate several timeouts. Besides, the parameter determines the maximum radius
of a data packet, that is, the maximum number of hops that a packet may travel, which is calculated by
the following formula:

<i>maximum radius = 2 * MIN(2 * maxNetworkDepth, 255)</i> \n

These uses of the parameter do not change if its value is greater than 15. Therefore to enable
transmitting data over long chains of devices, the parameter should be set to a real desired network depth,
rather than to an accidental value over 15.

The parameter should be the same on all devices in the network.

<b>Value range:</b>  from 0 to 255 \n
<b>C-type:</b> uint8_t \n
<b>Can be set:</b> at compile time only  \n
*/
#ifndef CS_MAX_NETWORK_DEPTH
#define CS_MAX_NETWORK_DEPTH                  5
#endif

//! \brief The method of automatic address assignment
/*!
If ::CS_NWK_UNIQUE_ADDR equals \c 0 this parameter is used to determine
the assignment method that is applied when a device enters the network to choose
a short address. Otherwise, the parameter is ignored.

<b>Value range:</b> \n
NWK_ADDR_ALLOC_DISTRIBUTED (equals 0) - distributed address allocation; the stack
applies a special recurrent algorithm to form a kind of a search tree from the network to simplify routing \n
NWK_ADDR_ALLOC_STOCHASTIC (equals 2) - the address is set to a random value, different
from all other short addresses in the network \n
NWK_ADDR_ALLOC_FROM_UID (equals 3) -  two lower bytes of the extended address are used
for the short address

<b>C-type:</b> uint8_t \n
<b>Can be set:</b> at compile time only \n
*/
#ifndef CS_ADDRESS_ASSIGNMENT_METHOD
#define CS_ADDRESS_ASSIGNMENT_METHOD          2
#endif


#if defined(AT86RF212) || defined(CUSTOMRF3)
/* Channel number range
  Page 0 (BPSK modulation is used and supported IEEE 802.15.4 2003/2006 )
    channel  0: 868 MHz      | 20 Kbit/sec
    channels 1 - 10: 915 MHz | 40 Kbit/sec
  Page 2 (O-QPSK modulation is used and supported IEEE 802.15.4 2006)
    channel  0: 868 MHz      | 100 Kbit/sec
    channels 1 - 10: 915 MHz | 250 Kbit/sec

  BOOST mode is enabled automatically only for 11, 10, 9, 8, 7, 6 dBm. All other power values do not use BOOST mode.
*/
//! \brief 32-bit mask of channels to be scanned before network is started
/*!
Channels that should be used are marked with logical 1 at corresponding bit
location.

\note For 900 MHz band you also need to specify channel page

<b>Value range:</b> 32-bit values: \n
Valid channel numbers for 2.4 GHz band are 0x0b - 0x1a \n
Valid channel numbers for 900 MHz band are 0x00 - 0x0a

<b>C-type:</b> uint32_t \n
<b>Can be set:</b> at any time before network start \n
*/
  #ifndef CS_CHANNEL_MASK
    #define CS_CHANNEL_MASK                       0x00000002L
  #endif
//! \brief Number of a channel page to be used
/*!
Channel page number defines band and modulation scheme that will be
used for communication.

<b>Value range:</b>
    0 -  915MHz (BPSK-40, channels 0x01 - 0x0a), 868MHz (BPSK-20, channel 0x00) \n
    2 -  915MHz (O-QPSK-250, channels 0x01 - 0x0a), 868Mhz (O-QPSK-100, channel 0x00) \n
    5 -  780MHz (O-QPSK-250, channels 0x00 - 0x03, Chinese band)

<b>C-type:</b> uint8_t \n
<b>Can be set:</b> at any time before network start \n
*/
  #ifndef CS_CHANNEL_PAGE
    #define CS_CHANNEL_PAGE                       0
  #endif

#else // AT86RF230/230B/231
//! \brief 32-bit mask of channels to be scanned before network is started
/*!
Channels that should be used are marked with logical 1 at corresponding bit
location.

\note For 900 MHz band you also need to specify channel page

<b>Value range:</b> 32-bit values: \n
Valid channel numbers for 2.4 GHz band are 0x0b - 0x1a \n
Valid channel numbers for 900 MHz band are 0x00 - 0x0a

<b>C-type:</b> uint32_t \n
<b>Can be set:</b> at any time before network start \n
*/
  #ifndef CS_CHANNEL_MASK
    #define CS_CHANNEL_MASK                       0x00010000L
  #endif
//! \brief Number of a channel page to be used. Ignored in the case of AT86RF230/230B/231 RF chip.
  #ifndef CS_CHANNEL_PAGE
    #define CS_CHANNEL_PAGE                       0
  #endif
#endif

//! \brief Extended PAN ID of the network to which the device should join
/*!
The parameter specifies the predefined extended PANID of the network to be formed
(for the coordinator) or joined (for a router or an end device). For a router or an end device
 the parameter can equal 0 allowing them to join the first suitable network that they discover.

<b>Value range:</b> All 64-bit values except for 0xFFFFFFFFFFFFFFFFLL;
specify a value in the \c 0x123456789ABCDEFLL format. \n
<b>C-type:</b> ExtPanId_t (equal to uint64_t)  \n
<b>Can be set:</b> at any time before network start \n
*/
#ifndef CS_EXT_PANID
#define CS_EXT_PANID                          0LL
#endif
//! \brief An actual value of the extended PANID after network has started
/*!
Is automatically written by the stack with an actual extended PANID of the network
to which the device joined. The parameter should not be changed while the device is
in the network.

<b>Value range:</b> All 64-bit values, specify a value in the \c 0x123456789ABCDEFLL format. \n
<b>C-type:</b> ExtPanId_t (equal to uint64_t)  \n
<b>Can be set:</b> at any time before network start \n
*/
#ifndef CS_NWK_EXT_PANID
#define CS_NWK_EXT_PANID                      0LL
#endif
/*! \brief Determines whether the static or automatic addressing mode will be used for the short address

If set to \c 1, the ::CS_NWK_ADDR will be used as the device's short address.
Otherwise, the short address is assigned automatically by the stack. An actual assignment
method is specified in ::CS_ADDRESS_ASSIGNMENT_METHOD.

<b>Value range:</b> \c 1 or \c 0 \n
<b>C-type:</b> bool \n
<b>Can be set:</b> at any time before network start \n
*/
#ifndef CS_NWK_UNIQUE_ADDR
#define CS_NWK_UNIQUE_ADDR                          0
#endif
//! \brief Device's short address if ::CS_NWK_UNIQUE_ADDR equals 1
/*!
If static addressing is applied the stack uses the value of the parameter as a short address. Otherwise,
the stack assigns the parameter to a randomly chosen value  unique within the network. In both cases
after the network start the parameter holds actual short address of the device. While the device is in
the network its value must not be changed.

Note that the coordinator short address always equals \c 0x0000.

<b>Value range:</b>  0x0000 - 0xFFF8 \n
<b>C-type:</b> ShortAddr_t \n
<b>Can be set:</b> at any time before network start \n
*/
#ifndef CS_NWK_ADDR
#define CS_NWK_ADDR                       0xFFFF
#endif
//! \brief End device sleep period given in milliseconds
/*!
On an end device this parameter determines the duration of a sleep period. Falling asleep is
performed with the ZDO_SleepReq() request. After sleeping period exceeds the node is awakened and
the application receives an indication via ZDO_WakeUpInd().
If the parameter's value is 0, then after the node falls asleep it can only be awakened by a
hardware interrupt; a callback for a given IRQ is registered via HAL_RegisterIrq().
On a router or the coordinator, the parameter is used in two ways:

1) To remove information about lost child end devices. If a parent receives no data polls or data
frames from the child end device for
CS_NWK_END_DEVICE_MAX_FAILURES*(CS_END_DEVICE_SLEEP_PERIOD + CS_INDIRECT_POLL_RATE) ms,
then it assumes it to be lost and deletes all information about such child.

2) To determine whether to store or drop a message addressed to a child end device.  The parent
estimates the time when its child end device will wake up by adding this value to the moment when the last poll
request has been received. If the time till end device wake up is greater than CS_MAC_TRANSACTION_TIME
the frame is stored. Otherwise, the frame is dropped.

<b>Value range:</b>  any value valid for the C-type; add "L" after a value \n
<b>C-type:</b> uint32_t \n
<b>Can be set:</b> at any time \n
*/
#ifndef CS_END_DEVICE_SLEEP_PERIOD
#define CS_END_DEVICE_SLEEP_PERIOD            10000L
#endif

//! \brief Full Function Device sleep period given in milliseconds
#ifndef CS_FFD_SLEEP_PERIOD
#define CS_FFD_SLEEP_PERIOD            10000L
#endif

//! \brief Encryption time of maximum-size packet in ms. Decryption time is equal to encryption time.
#ifndef CS_ENCRYPTION_TIME
#if defined(_SECURITY_)
  #ifdef _MAC_HW_AES_
    #define CS_ENCRYPTION_TIME 18     // HW encryption
  #else
    #define CS_ENCRYPTION_TIME 119    // SW encryption
  #endif
#else
  #define CS_ENCRYPTION_TIME 0ul
#endif
#endif

//! \brief Duration of internal processing of the maximum-size packet (without encryption)
#define CS_PACKET_PROCESS_INSIDE 7

//! \brief A period in ms of polling a parent for data by an end device
/*!
On a sleeping end device the parameter determines a period with which poll requests
are sent to the parent while the end device is awaken. A parent of a sleeping end device
uses the parameter to calculate estimates of the time when the next poll request from a child
will be received.

<b>Value range:</b>  any value valid for the C-type \n
<b>C-type:</b> uint32_t \n
<b>Can be set:</b> at any time \n
*/
#ifndef CS_INDIRECT_POLL_RATE
#define CS_INDIRECT_POLL_RATE                 1000
#endif

//! \brief Maximum amount of a failed sync frame.
/*!
Maximum amount of a failed sync frame. Before attempting to rejoin.

<b>C-type:</b> uint8_t \n
<b>Can be set:</b> at compile time only \n
*/
#ifndef CS_ZDO_MAX_SYNC_FAIL_AMOUNT
  #define CS_ZDO_MAX_SYNC_FAIL_AMOUNT            3
#endif

//! \brief The maximum number of retries of send frame to the parent.
/*!
The maximum number of retries of send frame to the parent. Before attempting to rejoin.

<b>C-type:</b> uint8_t \n
<b>Can be set:</b> at compile time only \n
*/
#ifndef CS_ZDO_PARENT_LINK_RETRY_THRESHOLD
  #define CS_ZDO_PARENT_LINK_RETRY_THRESHOLD     3
#endif



//! \brief A value used to calculate the length of time to spend scanning each channel
/*!
While scanning channels during network join the node keeps listening to each channel
specified by the ::CS_CHANNEL_MASK for a period of time calculated according to the
formula that for the 2.4GHz frequency band is: \n

<i>960 * 16 * (2 raised to a power n + 1) microseconds,</i>

providing n is a value of this parameter. Note that the formula for the Sub-GHz employs
another constant instead of 16.

<b>Value range:</b> 0x00-0x0e \n
<b>C-type:</b> uint8_t \n
<b>Can be set:</b> at any time \n
*/
#ifndef CS_SCAN_DURATION
#define CS_SCAN_DURATION                      0x05
#endif
//! \brief Specifies whether the device accepts children joining via MAC association
/*!
If the parameter being switched between \c 0xff and \c 0x00, determines whether the device
accepts or not a child joining the network via MAC association, that is, if the joining device does not
possess the PANID value of the network and its PANID parameter is set to 0.

The parameter can be set before the network start only. Once the device entered the network it can
only switch the parameter with the help of a permit duration ZDP request.

<b>Value range:</b> 0xff means "always on", 0x00 means "always off" \n
<b>C-type:</b> uint8_t \n
<b>Can be set:</b> at any time before network start \n
 */
#ifndef CS_PERMIT_DURATION
#define CS_PERMIT_DURATION                    0xff
#endif
//! \brief Enables or disables the multicast transmission mode at the NWK layer
/*!
If the parameter is set to \true multicasting on the NWK level is used, otherwise,
multicasting on the APS level is applied. The parameter is recommended to be set to
\c true. For detail refer to ZigBee specification.

<b>Value range:</b> \c true or \c false \n
<b>C-type:</b> bool \n
<b>Can be set:</b> at any time before network start \n
*/
#ifndef CS_NWK_USE_MULTICAST
#define CS_NWK_USE_MULTICAST                  true
#endif

//! \brief Indicates whether a complex descriptor is available on this device
/*!
<b>Value range:</b> \c true or \c false \n
<b>C-type:</b> bool \n
<b>Can be set:</b> at any time before network start \n
*/
#ifndef CS_COMPLEX_DESCRIPTOR_AVAILABLE
#define CS_COMPLEX_DESCRIPTOR_AVAILABLE       false
#endif

//! \brief Specifies whether a user descriptor is available on this device
/*!
A user descriptor is simply a string that can be assigned to a device and requested
by other devices in the network to identify the device. A user descriptor can read
and set on a remote device as well as on the local device with the help of corresponding
ZDP requests.

<b>Value range:</b> \c true or \c false \n
<b>C-type:</b> bool \n
<b>Can be set:</b> at any time before network start \n
*/
#ifndef CS_USER_DESCRIPTOR_AVAILABLE
#define CS_USER_DESCRIPTOR_AVAILABLE          true
#endif

//! \brief The stack version
/*!
This is a read-only parameter specifying the stack version
used by the device.

<b>C-type:</b> uint32_t \n
<b>Can be set:</b> at compile time only \n
*/
#ifndef CS_STACK_VERSION
#define CS_STACK_VERSION                      CS_STACK_VERSION_VALUE
#endif

#ifdef _ENABLE_PERSISTENT_SERVER_
#ifdef CS_PDS_STORING_INTERVAL
#warning "CS_PDS_STORING_INTERVAL parameter is deprecated. Use PDS_StoreByTimer() API instead."
#endif

//! \brief Enables or disables the power failure feature.
#ifdef CS_POWER_FAILURE
#warning "CS_POWER_FAILURE parameter is deprecated. Use PDS_StoreByTimer(), PDS_StoreByEvents(), PDS_Restore() API instead."
#endif
#endif /* _ENABLE_PERSISTENT_SERVER_ */

// by default in MAC = 7680L
//! \brief MAC transaction persistence time measured in ms
/*!
The parameter determines the maximum interval (in ms) a frame addressed to a sleeping
end device can be stored on the parent node. If the end device does not poll for data
during this time, then the frame is dropped.

<b>Value range:</b>  all unsinged 32-bit integers\n
<b>C-type:</b> uint32_t \n
<b>Can be set:</b> at any time \n
*/
#ifndef CS_MAC_TRANSACTION_TIME
#define CS_MAC_TRANSACTION_TIME               ((uint32_t)CS_END_DEVICE_SLEEP_PERIOD + ((uint32_t)CS_INDIRECT_POLL_RATE * 3ul))
#endif

//! \brief The size of the APS duplicate rejection table
/*!
The duplicate rejection table is used by APS to store information about incoming
unicast messages in order to reject messages that have been already received and processed.
Following ZigBee specification, the parameter should be not less than 1.

<b>Value range:</b>  greater than 1 \n
<b>C-type:</b> uint8_t \n
<b>Can be set:</b> at compile time only \n
*/
#ifndef CS_DUPLICATE_REJECTION_TABLE_SIZE
#define CS_DUPLICATE_REJECTION_TABLE_SIZE     10
#endif

//! \brief The size of the APS duplicate rejection table entry counter
/*!
This parameter determins how many APS frame counters can be tracked for duplication.

<b>Value range:</b>  greater than 1 \n
<b>C-type:</b> uint8_t \n
<b>Can be set:</b> at compile time only \n
*/
#ifndef CS_APS_DUPLICATE_ENTRY_COUNTER_SIZE
 #define CS_APS_DUPLICATE_ENTRY_COUNTER_SIZE 5
#elif CS_APS_DUPLICATE_ENTRY_COUNTER_SIZE > 32
  #undef  CS_APS_DUPLICATE_ENTRY_COUNTER_SIZE
  #define CS_APS_DUPLICATE_ENTRY_COUNTER_SIZE 32
  #warning  CS_APS_DUPLICATE_ENTRY_COUNTER_SIZE was set to 32
#endif

//! \brief The life time of the APS duplicate rejection table entry
/*!
This paramter determines the life time of APS frame counter entry for duplication

<b>Value range:</b>  greater than 1 \n
<b>C-type:</b> uint32_t \n
<b>Can be set:</b> at compile time only \n
*/
#ifndef CS_APS_DUPLICATE_ENTRY_LIFE_TIME
 #define CS_APS_DUPLICATE_ENTRY_LIFE_TIME 6800UL
#endif

//! \brief The maximum number of records in the NWK route table.
/*!
The parameter sets the maximum number of records that can be kept in the
NWK route table. The table is used by NWK to store information about established
routes. Each table entry specifies the next-hop short address for a route from the
current node to a given destination node. The table is being filled automatically
during route discovery. An entry is added when a route is discovered.

Since the end device always sends a frame directly to its parent its route table size
should be set to 0.

<b>C-type:</b> uint16_t \n
<b>Can be set:</b> at compile time only \n
*/
#if defined _ROUTER_ || defined _COORDINATOR_
  #if !defined CS_ROUTE_TABLE_SIZE
    #define CS_ROUTE_TABLE_SIZE               4
  #endif
  #if CS_ROUTE_TABLE_SIZE == 0
    #undef  CS_ROUTE_TABLE_SIZE
    #define CS_ROUTE_TABLE_SIZE               1
    #warning CS_ROUTE_TABLE_SIZE was set to 1
  #endif
#else // _ENDDEVICE_
  #undef CS_ROUTE_TABLE_SIZE
  #define CS_ROUTE_TABLE_SIZE                 0
#endif // _ROUTER_ or _COORDINATOR_

//! \brief The maximum number of records in the NWK address map table
/*!
The parameter sets the maximum number of records in the address map table used by
NWK to store pairs of corresponding short and extended addresses. The stack appeals
to the table when a data frame is being sent to a specified extended address to extract
the corresponding short address. If it fails to find the short address, an error is reported.

<b>C-type:</b> NwkSizeOfAddressMap_t (typedef for uint16_t) \n
<b>Can be set:</b> at compile time only \n
*/
#ifndef CS_ADDRESS_MAP_TABLE_SIZE
#define CS_ADDRESS_MAP_TABLE_SIZE             5
#endif
#if CS_ADDRESS_MAP_TABLE_SIZE == 0
  #undef  CS_ADDRESS_MAP_TABLE_SIZE
  #define CS_ADDRESS_MAP_TABLE_SIZE           1
  #warning CS_ADDRESS_MAP_TABLE_SIZE was set to 1
#endif

//! \brief The maximum number of records in the NWK route discovery table
/*!
The parameter specifies the size of the route discovery table used by NWK to store
next-hop addresses of the nodes for routes that are not yet established. Upon exhausting
the capacity of the table, the stack starts rewriting old entries. If the size of the route table
is big enough after all used routes are established the table may not be used.

Since the end device always sends a frame directly to its parent its route discovery table size
should be set to 0.

<b>C-type:</b> uint8_t \n
<b>Can be set:</b> at compile time only \n
*/
#if defined _ROUTER_ || defined _COORDINATOR_
  #if !defined CS_ROUTE_DISCOVERY_TABLE_SIZE
    #define CS_ROUTE_DISCOVERY_TABLE_SIZE     3
  #endif
  #if CS_ROUTE_DISCOVERY_TABLE_SIZE == 0
    #undef  CS_ROUTE_DISCOVERY_TABLE_SIZE
    #define CS_ROUTE_DISCOVERY_TABLE_SIZE     1
    #warning CS_ROUTE_DISCOVERY_TABLE_SIZE was set to 1
  #endif
#else // _ENDDEVICE_
  #undef CS_ROUTE_DISCOVERY_TABLE_SIZE
  #define CS_ROUTE_DISCOVERY_TABLE_SIZE       0
#endif // _ROUTER_ or _COORDINATOR_

/*
Amount of buffers on NWK layer used to keep incoming and outgoing frames.
This parameters affects how many children of a parent are able to get broadcat messages.
*/
#if !defined CS_NWK_BUFFERS_AMOUNT
  #define CS_NWK_BUFFERS_AMOUNT               4
#endif // CS_NWK_BUFFERS_AMOUNT

//!\brief The size of the passive acknoledgement table
#if !defined CS_NWK_PASSIVE_ACK_AMOUNT
  #define CS_NWK_PASSIVE_ACK_AMOUNT           8
#endif // CS_NWK_PASSIVE_ACK_AMOUNT

//!\brief The size of of the broadcast transmission table
/*!
The broadcast transmission table is used for tracking incoming broadcast messages
to mark messages that have already been processed by the node. This causes only one
copy for each broadcast message to be processed. An entry for a broadcast message is
stored for a certain period of time and then removed.

<b>C-type:</b> uint8_t \n
<b>Can be set:</b> at compile time only \n
*/
#if !defined CS_NWK_BTT_SIZE
  #define CS_NWK_BTT_SIZE                     8
#endif

//!\brief The size of of the group table
/*!
The size cannot be 0. The group table stores pairs of a group address and an endpoint. Upon
receiving a frame addressed to members of a certain group which include the current node as well
the stack fires indications on all endpoints registered with the group address.

<b>C-type:</b> uint8_t \n
<b>Can be set:</b> at compile time only \n
*/
#if defined CS_GROUP_TABLE_SIZE
  #if CS_GROUP_TABLE_SIZE == 0
    #undef CS_GROUP_TABLE_SIZE
    #define CS_GROUP_TABLE_SIZE               1
    #warning CS_GROUP_TABLE_SIZE was set to 1
  #endif
#else
  #define CS_GROUP_TABLE_SIZE                 1
#endif

//! \brief The number of buffers for data requests on the APS layer
/*!
The parameter specifies the number of buffers that are allocated by APS
to store data requests parameters. The parameter puts an upper bound to the number of
data requests that can be processed by APS simultaneously. If all buffers are in
use and a new data request appears, it is kept in a queue until a buffer is released.

<b>C-type:</b> uint8_t \n
<b>Can be set:</b> at compile time only \n
*/
#ifndef CS_APS_DATA_REQ_BUFFERS_AMOUNT
  #ifdef _SECURITY_
    #define CS_APS_DATA_REQ_BUFFERS_AMOUNT       3
  #else // !_SECURITY_
    #define CS_APS_DATA_REQ_BUFFERS_AMOUNT       2
  #endif // _SECURITY_
#endif

//! \brief The number of buffers for acknowledgement messages sent by APS
/*!
This parameter determines the amount of memory that needs to be allocated for a special type
of buffers used by APS to store payloads for acknowledgement frames. The need to use the buffers
occurs when the node receives a frame that has to be acknowledged. That is, the APS component on
the node has to send an acknowledgement frame. For frames initiated by the application, the memory for
a payload is to be allocated by the application on its own, while the payload memory for an acknowledgement
frame shall be reserved by APS. The request parameters are still stored in the data request buffers.

Typically, a value of this parameter equals ::CS_APS_DATA_REQ_BUFFERS_AMOUNT - 1.

<b>C-type:</b> uint8_t \n
<b>Can be set:</b> at compile time only \n
*/
//  To send APS ACK both types of buffers are used (.._DATA_REQ_.. and .._ACK_FRAME_..)
#ifndef CS_APS_ACK_FRAME_BUFFERS_AMOUNT
  #define CS_APS_ACK_FRAME_BUFFERS_AMOUNT        2
#endif

//! \brief The number of transmission retries to be performed at the APS layer
/*!
The parameter sets the number of attempts that will be made by APS layer to
transmit a data frame. If all these attempts fail due to underlying layers failures,
then APS response with an error status. The APS performs retries only for frames that
require APS Acknowledgement.

<b>Value range:</b>  from 1 to 5 \n
<b>C-type:</b> uint8_t \n
<b>Can be set:</b> at any time, but shall not be done during data exchange \n
*/
#ifndef CS_APS_MAX_FRAME_RETRIES
  #define CS_APS_MAX_FRAME_RETRIES     3
#elif CS_APS_MAX_FRAME_RETRIES > 5
  #undef  CS_APS_MAX_FRAME_RETRIES
  #define CS_APS_MAX_FRAME_RETRIES     5
  #warning CS_APS_MAX_FRAME_RETRIES was set to 5
#elif CS_APS_MAX_FRAME_RETRIES == 0
  #undef  CS_APS_MAX_FRAME_RETRIES
  #define CS_APS_MAX_FRAME_RETRIES     1
  #warning CS_APS_MAX_FRAME_RETRIES was set to 0
#endif

//! \brief Allows the stack to be waked up by the DTR line of the UART interface
/*!
If the parameter is set to true, the stack will wake up if a signal is received
via the DTR line of UART. If set to false, the feature is disabled.

<b>Value range:</b> \c true of \c false \n
<b>C-type:</b> bool \n
<b>Can be set:</b> at any time before network start \n
*/
#ifndef CS_DTR_WAKEUP
#define CS_DTR_WAKEUP                         false
#endif

/*! \brief If the number of consecutive link status frames given by this parameter is missed
from a neighbor it is removed from the neighbor table

For all neighbors except for end device children the stack tracks the time of receiving link statuses.
If link statuses are not received from a given neighbor for this parameter's value times of link status
period (typically 15 seconds), then the neighbor is deleted from the neighbor table.

<b>C-type:</b> uint8_t \n
<b>Can be set:</b> at any time \n
*/
#ifndef CS_NWK_MAX_LINK_STATUS_FAILURES
#define CS_NWK_MAX_LINK_STATUS_FAILURES   3
#endif

/*!  \brief Is used to calculate the length of time after which a not responding
end device child is considered lost

A sleeping end device is considered lost and a corresponding notification is raised on the
parent, if the end device does not polls for data for the time span which duration is calculated by
the following formula:
\code
CS_NWK_END_DEVICE_MAX_FAILURES * (CS_END_DEVICE_SLEEP_PERIOD + CS_INDIRECT_POLL_RATE)
\endcode

<b>C-type:</b> uint8_t \n
<b>Can be set:</b> at any time \n
 */
#ifndef CS_NWK_END_DEVICE_MAX_FAILURES
#define CS_NWK_END_DEVICE_MAX_FAILURES    3
#endif

//! \brief The maximum number of records in the binding table
/*!
The parameter sets the size of the binding table used by APS to store binding
links, which are structures containing information about source and destination
extended addresses and endpoints for unicast bindings and just group addresses
as destinations for group bindings. If the binding is going to be applied to send
a data frame, then the corresponding entry shall be first inserted into the table via
the APS_BindingReq() function.

<b>C-type:</b> uint8_t \n
<b>Can be set:</b> at compile time only \n
*/
#ifdef _BINDING_
#ifndef CS_APS_BINDING_TABLE_SIZE
  #define CS_APS_BINDING_TABLE_SIZE       1
#endif
#endif //_BINDING_

#ifdef _APS_FRAGMENTATION_
//! \brief The maximum number of blocks the asdu can be split into
/*!
This parameter limits the number of pieces to which the data sent with one APS request
can be split i f the fragmentation feature is applied. Thus it also limits the maximum amount
of data sent by the application  with a single request:\n
maximum data length = ::CS_APS_MAX_BLOCKS_AMOUNT * ::CS_APS_BLOCK_SIZE if the latter
parameter is not 0, else \n
maximum data length = ::CS_APS_MAX_BLOCKS_AMOUNT  * ::APS_MAX_ASDU_SIZE.

<b>C-type:</b> uint8_t \n
<b>Can be set:</b> at compile time only \n
 */
#ifndef CS_APS_MAX_BLOCKS_AMOUNT
#define CS_APS_MAX_BLOCKS_AMOUNT 0
#endif

//! \brief The block size that is used for fragments in fragmented transmission/reception
/*!
If the value is 0, blocks will be of the maximum possilbe size; that is the size will
equal the value of ::CS_APS_MAX_ASDU_SIZE. The parameter and ::CS_APS_MAX_BLOCKS_AMOUNT are
also used to determine an amount of memory allocated for a special buffer that keeps parts of
incoming fragmented message until all of them are received.

<b>C-type:</b> uint16_t \n
<b>Can be set:</b> at compile time only \n
*/
#ifndef CS_APS_BLOCK_SIZE
  #define CS_APS_BLOCK_SIZE 0
#endif

//! \brief The size of a single fragment in fragmented transmission of a data frame
/*!
The parameter is used to determine the size of fragments into which a payload
of a data frame should be split before transmission if fragmentation is applied.

<b>Value range</b>: 1 - ::APS_MAX_ASDU_SIZE \n
<b>C-type:</b> uint16_t \n
<b>Can be set:</b> at compile time only
*/
#if !defined CS_APS_TX_BLOCK_SIZE
  #define CS_APS_TX_BLOCK_SIZE CS_APS_BLOCK_SIZE
#endif
#if ((CS_APS_TX_BLOCK_SIZE > APS_MAX_ASDU_SIZE) || (CS_APS_TX_BLOCK_SIZE == 0))
  #undef CS_APS_TX_BLOCK_SIZE
  #define CS_APS_TX_BLOCK_SIZE APS_MAX_ASDU_SIZE
#endif

//! \brief The size of a single fragment in fragmented reception of a data frame
/*!
The parameter is used to define the size of a buffer to keep received pieces
of a fragmented data frame. An actual size of received fragments can be smaller
than this parameter.

The parameter can be greater that the size of transmitted fragments set be
::CS_APS_TX_BLOCK_SIZE.

<b>Value range</b>: 1 - ::APS_MAX_RX_ASDU_SIZE \n
<b>C-type:</b> uint16_t \n
<b>Can be set:</b> at compile time only
*/
#if !defined CS_APS_RX_BLOCK_SIZE
  #define CS_APS_RX_BLOCK_SIZE CS_APS_BLOCK_SIZE
#endif
#if ((CS_APS_RX_BLOCK_SIZE > APS_MAX_RX_ASDU_SIZE) || (CS_APS_RX_BLOCK_SIZE == 0))
  #undef CS_APS_RX_BLOCK_SIZE
  #define CS_APS_RX_BLOCK_SIZE APS_MAX_RX_ASDU_SIZE
#endif

//! \brief Maximum transmission window size (in blocks)
/*!
The parameter determines how many pieces of a fragmented message are sent before
waiting for an aknowledgment. After acknowledgement is received another
portion of frames are sent and so on.

<b>Value range:</b> greater than 0 \n
<b>C-type:</b> uint8_t \n
<b>Can be set:</b> at any time before network start \n
*/
#ifndef CS_APS_MAX_TRANSMISSION_WINDOW_SIZE
  #ifdef _ZHP_SECURITY_
    #define CS_APS_MAX_TRANSMISSION_WINDOW_SIZE        1
  #else
    #define CS_APS_MAX_TRANSMISSION_WINDOW_SIZE        3
  #endif
#endif

#endif /* _APS_FRAGMENTATION_  */


/*****************************************************************************
  Security related defines
*****************************************************************************/

#ifdef _SECURITY_
/*! \brief Turns on/off security features

The parameter shall be configured before the network start. If it is set
to \c false, authentication is not applied as well as encryption of data frames,
therefore the device will only be able to enter networks without security.

<b>Value range:</b> \c true of \c false \n
<b>C-type:</b> bool \n
<b>Can be set:</b> at any time before network start \n
*/
#ifndef CS_SECURITY_ON
#define CS_SECURITY_ON  true
#endif

/*! \brief A timeout for authentication procedures

A timeout is started when connection with a parent is established. If the
security related procedures that are performed after this will not be completed
before the timeout exceeds, the device will fail joining the network. A value is
measured  in milliseconds.

<b>C-type:</b> uint32_t \n
<b>Can be set:</b> at compile time only \n
*/
#ifndef CS_APS_SECURITY_TIMEOUT_PERIOD
#define CS_APS_SECURITY_TIMEOUT_PERIOD        (2 * 1000 * 10)
#endif

//! \brief Default exteded address of the trust center
/*!
The parameter specifies the trust center extended address. The stack makes use of the
parameter to support various operations in networks with security enabled. For correct network operation
the parameter's value must coincide with the actual trust center address.

In case the trust center extended address is unknown, for example, for testing purposes, the
parameter can be set to the universal trust center address which equals ::APS_UNIVERSAL_EXTENDED_ADDRESS.

<b>Value range:</b> All 64-bit values except for 0xFFFFFFFFFFFFFFFFLL,
specify a value in the \c 0x123456789ABCDEFLL format. \n
<b>C-type:</b> ExtAddr_t \n
<b>Can be set:</b> at any time before network start \n
*/
#ifndef CS_APS_TRUST_CENTER_ADDRESS
#define CS_APS_TRUST_CENTER_ADDRESS           0xAAAAAAAAAAAAAAAALL
#endif

//! \brief Default short address of the trust center
/*!
If the parameter is set to \c 0xFFFF the feature is not used. If any other
valid short address value is set the, it is considered the short address of
the trust center. If the trust center has a different short address the current
node will not be able to join such network. Therefore the parameter should
be used only when the real trust center address is known for sure.

<b>Value range:</b>  0x0000 - 0xFFF8 and 0xFFFF \n
<b>C-type:</b> ShortAddr_t \n
<b>Can be set:</b> at any time before network start \n
*/
#ifndef CS_SHORT_TRUST_CENTER_ADDRESS
#define CS_SHORT_TRUST_CENTER_ADDRESS         0xFFFF
#endif

//! \brief ZDO security status
/*!
The parameter is used to determine the security type.

<b>Value range:</b> 0,3 - for standard security; 1 - for standard link security; 1,2,3 - for high security. \n
0 - network key is preconfigured \n
1 - network join with the trust center link key, which must be set via APS_SetLinkKey() \n
2 - network join with the trust center link key generated during SKKE procedure from
    the trust center master key, which is set via APS_SetMasterKey() \n
3 - <i>for standard security:</i>network key is not preconfigured but is received from the
    trust center in an unencrypted frame;
    <i>for high security:</i>the trust center master key is not preconfigured but is received
    from the trust center during network join.

\note High security mode is not supported by publicly available BitCloud packages.

<b>C-type:</b> uint8_t \n
<b>Can be set:</b> at any time before network start \n
 */
#ifndef CS_ZDO_SECURITY_STATUS
#define CS_ZDO_SECURITY_STATUS                0
#endif
/*! \brief The maximum number of network keys that can be stored on the device

A device in a secured network can keep several network keys up to the value of this
parameter. Upon frame reception the device extracts key sequence number from the auxiliary
header of the frame and decrypts the message with the network key corresponding to this
sequence number. Besides, one key is considered active for each device; this is the key that
is used for encrypting outgoing frames.

Network keys are distributed by the trust center with the help of the APS_TransportKeyReq()
command. A switch to a new active network key is also initiated by the trust center, which
should use the APS_SwitchKeyReq() function.

<b>C-type:</b> NwkKeyAmount_t (typedef for uint8_t) \n
<b>Can be set:</b> at compile time only \n
*/
#ifndef CS_NWK_SECURITY_KEYS_AMOUNT
#define CS_NWK_SECURITY_KEYS_AMOUNT           2
#endif
/*! \brief Default value of the network key

This is the parameter to store a value of the network key which is used to encrypt a part of a
data frame occupied by the NWK payload. This type of encryption is applied in all types of
security modes (except for no security mode). For standard security with link keys
encryption of the APS payload with a link key may also be enabled by setting the
\c txOptions.securityEnabledTransmission field in the APS request parameters to 1, but note that
if the \c txOptions.useNwkKey field is set to 0, the APS payload is encrypted
with the network key.

The network key must be predefined if standard security is used with ::CS_ZDO_SECURITY_STATUS
set to 0. For all other values of ::CS_ZDO_SECURITY_STATUS the network key is received from the
trust center during device authentication. Note that in the standard security mode with
::CS_ZDO_SECURITY_STATUS equal to 3 the network key is transferred to the joining device
in an unencrypted frame.

<b>Value range:</b> all 128-bit values \n
<b>C-type:</b> uint8_t[16]  or any compatible \n
<b>Can be set:</b> at any time before network start \n
*/
#ifndef CS_NETWORK_KEY
#define CS_NETWORK_KEY                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
#endif
// \cond internal
// Default security level.When calculating APS_MAX_ASDU_SIZE assumed that this parameter is 5.
#ifndef CS_SECURITY_LEVEL
#define CS_SECURITY_LEVEL             0x05U
#endif
#if CS_SECURITY_LEVEL != 0x05U
#error Parameter CS_SECURITY_LEVEL must be 5.
#endif

// Default nwkSecureAllFrames attribute value in NIB.
#ifndef CS_SECURITY_ALL_FRAMES
#define CS_SECURITY_ALL_FRAMES        true
#endif
// \endcond
/*! \brief The maximum number of authentication requests that the trust center can process simultaneously

The parameter is used on the trust center to allocate memory for buffers used
during joining device authentication. A value of the parameter determines how many
authentication request the stack on the trust center can process at once.

<b>C-type:</b> uint8_t \n
<b>Can be set:</b> at compile time only \n
*/
#ifndef CS_MAX_TC_AUTHENTIC_PROCESS_AMOUNT
  #define CS_MAX_TC_AUTHENTIC_PROCESS_AMOUNT     1
#endif

#ifdef _LINK_SECURITY_
//! \brief The maximum number of entries in APS key-pair set
/*!
The parameter is used in standard security with link keys to specify the size of the
APS key-pair set. The APS key-pair set stores pairs of corresponding extended address
and a link key or a master key. For each node with which the current node is going to
communicate it must keep an entry with the remote node extended address and a link key.

If the link key for communication with a remote node is unknown, the node can request
it from the trust center via the APS_RequestKeyReq() function. If the trust center stores
the requested link key for this pair of node it will send it to both nodes.

New entries can be added to the APS key-pair set manually using the APS_SetLinkKey()
function.

<b>Value range:</b> 1 - 32767 \n
<b>C-type:</b> uint16_t \n
<b>Can be set:</b> at compile time only \n
*/
#ifndef CS_APS_KEY_PAIR_DESCRIPTORS_AMOUNT
#define CS_APS_KEY_PAIR_DESCRIPTORS_AMOUNT  3
#endif

//\cond internal
//Not used in the stack
#ifndef CS_MASTER_KEY
#define CS_MASTER_KEY                 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
#endif

#ifndef CS_MAX_TC_ESTABLISH_KEY_AMOUNT
#define CS_MAX_TC_ESTABLISH_KEY_AMOUNT      1
#endif
//\endcond
#endif

#endif

#ifdef _PERMISSIONS_
//! \brief The maximum amount of records in the permissions table
/*!
The parameter sets the size of the table that stores permissions for individual
devices. To add permissions for a device with a given extended address use the
ZDO_SetPermissions() function, which adds a new record to the permissions table.

<b>Value range:</b> 0 - 255 \n
<b>C-type:</b> uint8_t \n
<b>Can be set:</b> at compile time only \n
*/
  #ifndef CS_PERMISSIONS_TABLE_SIZE
    #define CS_PERMISSIONS_TABLE_SIZE            5
  #endif
#endif

/*! \brief The number of network join attempts ZDO performs before it reports failure

Determines the maximum number of attempts to enter a network performed by the stack
during network start. Upon each attempt ZDO sends a beacon request and collects
beacon responses from nearby devices all over again. A pause length between two attempts
is given by the ::CS_ZDO_JOIN_INTERVAL parameter.

<b>C-type:</b> uint8_t \n
<b>Can be set:</b> at any time before network start \n
*/
#ifndef CS_ZDO_JOIN_ATTEMPTS
#define CS_ZDO_JOIN_ATTEMPTS                4
#endif

/*! \brief The interval between two network join attempts

The parameter specifies the time span in milliseconds between two attempts
to join the network. See ::CS_ZDO_JOIN_ATTEMPTS description.

<b>C-type:</b> uint32_t \n
<b>Can be set:</b> at any time before network start \n
*/
#ifndef CS_ZDO_JOIN_INTERVAL
#define CS_ZDO_JOIN_INTERVAL                1000
#endif

/*! \brief ZDP response awaiting timeout in milliseconds. To use automatically
calculated value, set to 0

The parameter determines the length of time for which the reply in response to a ZDP request
is awaited. If no response is received before the timeout exceeds the confirmation callback for
the ZDP request is called to report a failure status.

By default, the parameter is set to the automatically calculated value is used
(since the parameter is set to 0). It is assumed that ZDP requests do not have to reach sleeping
devices. That is why the automatically calculated value for the parameter does not depend
on CS_ZDO_SLEEP_PERIOD, which specifies the length of the sleeping period, and the value is only
as large as to allow active devices to reply.

<b>C-type:</b> uint16_t \n
<b>Can be set:</b> at any time before network start \n
*/
#ifndef CS_ZDP_RESPONSE_TIMEOUT
#define CS_ZDP_RESPONSE_TIMEOUT             0
#endif

//! \brief TC Keep-Alive polling interval, should be in the range 5-20 minutes.
#ifndef CS_ZDO_TC_KEEP_ALIVE_INTERVAL
#define CS_ZDO_TC_KEEP_ALIVE_INTERVAL       20
#endif


//! \brief  Indication to use a predefined network PANID value
/*!
If a predefined value is required to be used for the network PANID, the parameter
shall be set to \c true. Otherwise, it shall be set to \c false.

<b>Value range:</b> \c true or \c false \n
<b>C-type:</b> bool \n
<b>Can be set:</b> at any time before network start \n
*/
#ifndef CS_NWK_PREDEFINED_PANID
#define CS_NWK_PREDEFINED_PANID false
#endif

//! \brief  A predefined short PANID value
/*!
The parameter holds the short PANID value generated randomly if ::CS_NWK_PREDEFINED_PANID
equals \c false. Otherwise, the predefined parameter's value is
used as the short PANID.

<b>Value range:</b> 16-bit values from the range 0x0000 - 0xFFFE \n
<b>C-type:</b> PanId_t (typedef for uint16_t) \n
<b>Can be set:</b> at any time before network start \n
*/
#ifndef CS_NWK_PANID
#define CS_NWK_PANID    0x1234
#endif
// \cond internal
/*! 16-bit manufacturer code allocated by the ZigBee Alliance.
 * See ZigBee Manufacturer Code Database, 053874r16, Atmel code. */
#ifndef CS_MANUFACTURER_CODE
#define CS_MANUFACTURER_CODE  0x1014
#endif

#if defined _ROUTER_ || defined _COORDINATOR_
  #if !defined CS_JOIN_IND_OBJ_AMOUNT
    #define CS_JOIN_IND_OBJ_AMOUNT          (CS_NWK_BUFFERS_AMOUNT >> 1)
  #endif
#else // _ENDDEVICE_
  #undef   CS_JOIN_IND_OBJ_AMOUNT
  #define  CS_JOIN_IND_OBJ_AMOUNT           0
#endif // _ROUTER_ or _COORDINATOR_

/* Maximum number of records in the route cache */
#if defined _NWK_ROUTE_CACHE_ && (defined _ROUTER_ || defined _COORDINATOR_)
  #if !defined CS_ROUTE_CACHE_SIZE
    #define CS_ROUTE_CACHE_SIZE             4
  #endif /* CS_ROUTE_CACHE_SIZE */
  #if CS_ROUTE_CACHE_SIZE == 0
    #undef  CS_ROUTE_CACHE_SIZE
    #define CS_ROUTE_CACHE_SIZE     1
    #warning CS_ROUTE_CACHE_SIZE was set to 1
  #endif
#else /* no route cache */
  #undef  CS_ROUTE_CACHE_SIZE
  #define CS_ROUTE_CACHE_SIZE               0
#endif

/** The time in milliseconds between concentrator route discoveries.
 * ZigBee spec r18, Table 3.44, page 348. */
#if !defined NWK_CONCENTRATOR_DISCOVERY_TIME
  #define NWK_CONCENTRATOR_DISCOVERY_TIME  20000UL /* milliseconds */
#endif

// \endcond
#endif  //_MAC2_

#if ZCL_SUPPORT == 1
/** \brief The number of buffers allocated for outgoing and received command frames

The size of an individual buffer is given by ::CS_ZCL_BUFFER_SIZE.

<b>Value range:</b> 0 to  255 \n
<b>C-type:</b> uint8_t \n
<b>Can be set:</b> at compile time only
*/
#ifndef CS_ZCL_MEMORY_BUFFERS_AMOUNT
  #define CS_ZCL_MEMORY_BUFFERS_AMOUNT           5
#endif

/** \brief The size of a buffer used by ZCL to store command frames

When the application sends a command to ZCL, it copies the whole the
whole packet provided by the application to a buffer. Therefore, even when
fragmentation is enabled a ZCL buffer should be big enough to hold
the whole packet.

<b>Value range:</b> 0 to  255 \n
<b>C-type:</b> uint8_t \n
<b>Can be set:</b> at compile time only
*/
#ifndef CS_ZCL_BUFFER_SIZE
  #define CS_ZCL_BUFFER_SIZE (APS_MAX_ASDU_SIZE)
#endif

#if APP_USE_OTAU == 1
/** \brief The default address of an upgrade server

The parameter indicates how the OTAU client will search for OTAU servers in the network.
If one of broadcast addresses is specified, the client will attempt to find all devices supporting
the OTAU server cluster and will request new images from the first server that will respond.
Otherwise, the client will try to connect to a particular device with the specified extended
address.

The parameter is valid only for OTAU clients.

<b>Value range:</b> any 64-bit value: \n
\li \c 0x0000000000000000ull, \c 0xFFFFFFFFFFFFFFFFull - a server discovery request is broadcast
\li otherwise, the client tries to connect to a particular node

<b>C-type:</b> ExtAddr_t \n
<b>Can be set:</b> at any time before an OTAU start \n
*/
#ifndef CS_ZCL_OTAU_DEFAULT_UPGRADE_SERVER_IEEE_ADDRESS
  #define CS_ZCL_OTAU_DEFAULT_UPGRADE_SERVER_IEEE_ADDRESS          0xFFFFFFFFFFFFFFFFull
#endif
/** \brief The interval in milliseconds between two attempts to find an upgrade server

The parameter is valid only for OTAU clients.

<b>Value range:</b> any 32-bit value \n
<b>C-type:</b> uint32_t \n
<b>Can be set:</b> at any time before an OTAU start \n
*/
#ifndef CS_ZCL_OTAU_SERVER_DISCOVERY_PERIOD
  #define CS_ZCL_OTAU_SERVER_DISCOVERY_PERIOD                      60000ul
#endif
/** Default values of OTAU session. */
/** \brief The amount of servers the OTAU client can listen to during upgrade server discovery

The OTAU client can store information about several discovered OTAU servers. However, the client
device tries to connect to discovered servers one by one until the first successful attempt and then
communicates and uploads the whole image from the first suitable server.

The parameter is valid only for OTAU clients.

<b>Value range:</b> at least 1 \n
<b>C-type:</b> uint8_t \n
<b>Can be set:</b> at compile time only \n
*/
#ifndef CS_ZCL_OTAU_DISCOVERED_SERVER_AMOUNT
  #define CS_ZCL_OTAU_DISCOVERED_SERVER_AMOUNT                     1
#endif
/** \brief The number of clients that the OTAU server can serve simultaneously

If this parameter equals 1, the OTAU server will upgrade devices in the network
one by one. However, the server can process more than one client sessions at a time,
if this parameter is greater than 1.

The parameter is valid only for OTAU servers.

<b>Value range:</b> at least 1 \n
<b>C-type:</b> uint8_t \n
<b>Can be set:</b> at compile time only \n
*/
#ifndef CS_ZCL_OTAU_CLIENT_SESSION_AMOUNT
  #define CS_ZCL_OTAU_CLIENT_SESSION_AMOUNT                        1
#endif
/** \brief Indicates that image page request are used to load an image

If the parameter is set to \c true the OTAU client will use image page requests
(rather than image <i>block</i> requests) to load an application image from the
OTAU server. In this case the server will send a whole page (consisting of
multiple blocks) without requesting an APS acknowledgement for each block. The
client marks not-delivered blocks and sends an image block request for each of
missed blocks after the transmission on the page finishes.

If the parameter is set to \c false image block requests will be used. The APS
acknowledgement is sent for each of received blocks. If any of the blocks is
missing loading of the image is restarted.

The parameter is valid only for OTAU clients.

<b>Value range:</b> \c true or \c false \n
<b>C-type:</b> uint8_t \n
<b>Can be set:</b> at compile time only \n
*/
#ifndef CS_ZCL_OTAU_IMAGE_PAGE_REQUEST_ENABLE
  #define CS_ZCL_OTAU_IMAGE_PAGE_REQUEST_ENABLE                     1
#endif
/** \brief Period in milliseconds between two image block responses sent from the OTAU server

The parameter indicates how fast the OTAU server shall send data (via the image block response command)
to the OTAU client. The value is set on the client. The server receives the value from the
client. The server shall wait for, at minimum, the period specified in the parameter before sending
another block to the client.

The parameter is valid only for OTAU clients.

<b>Value range:</b> not less than 200ms (according to the OTAU specification) \n
<b>C-type:</b> uint16_t \n
<b>Can be set:</b> at compile time only \n
*/
#ifndef CS_ZCL_OTAU_IMAGE_PAGE_REQUEST_RESPONSE_SPACING
  #define CS_ZCL_OTAU_IMAGE_PAGE_REQUEST_RESPONSE_SPACING          200
#endif
/** \brief The page size - the number of bytes sent for a single image page request

The parameter sets the number of bytes to be sent by the server for an image page
request sent by the OTAU client. A page is transmitted in several data frames, one
data frame for each block of data. The client, upon receiving individual blocks,
does not send an APS acknowledgement until all blocks for the current page are not
sent. Blocks that have not been delivered are requested by the client via image
block requests after loading of the current page finishes.

The page size should be greater than or equal to 64 bytes.

The parameter is valid only for OTAU clients.

<b>Value range:</b> equal to or greater than 64 \n
<b>C-type:</b> uint16_t \n
<b>Can be set:</b> at compile time only \n
*/
#if CS_ZCL_OTAU_IMAGE_PAGE_REQUEST_ENABLE == 1
  #ifndef CS_ZCL_OTAU_IMAGE_PAGE_REQUEST_PAGE_SIZE
    #define CS_ZCL_OTAU_IMAGE_PAGE_REQUEST_PAGE_SIZE               256
  #endif
#else
  #undef CS_ZCL_OTAU_IMAGE_PAGE_REQUEST_PAGE_SIZE
  #define CS_ZCL_OTAU_IMAGE_PAGE_REQUEST_PAGE_SIZE                 1
#endif //CS_ZCL_OTAU_IMAGE_PAGE_REQUEST_ENABLE == 1

#endif // APP_USE_OTAU == 1
#endif // ZCL_SUPPORT == 1

#ifndef CS_BAN_TABLE_SIZE
  #define CS_BAN_TABLE_SIZE  1
#endif /* _MAC_BAN_NODE_ */

/** \brief The maximum route cost between two nodes for direct delivery

A data frame is routed directly to the neighbor only if the route cost, read
from the neighbor table, is less than the value of the parameter. If the route
cost is greater than the value of the parameter, route discovery will be
initiated to find an indirect route to the neighbor.

<b>Value range:</b> 0 - 8 \n
0 - ignore information in the neighbor table (always start route discovery) \n
8 - always send data directly to a neighbor

<b>C-type:</b> NwkLinkCost_t (typedef for uint8_t) \n
<b>Can be set:</b> at any time before network start \n
 */
#ifndef CS_MAX_NEIGHBOR_ROUTE_COST
  #define CS_MAX_NEIGHBOR_ROUTE_COST       5U
#endif

/** \brief Sets the upper bound on one-hop route costs that are
taken into account during route discovery

One-hop routes (routes between two neighbor nodes) with costs greater
or equal to this parameter are ignored. This parameter allows to filter
links, eliminating those with greater costs, using only more stable ones.

<b>Value range:</b> 0 - 8 \n
0 - ignore all links (no data flow) \n
8 - accept all links

<b>C-type:</b> NwkLinkCost_t (typedef for uint8_t) \n
<b>Can be set:</b> at any time before network start \n
*/
#ifndef CS_MAX_LINK_ROUTE_COST
  #define CS_MAX_LINK_ROUTE_COST           8U
#endif

/** \brief This flag determines whether or not a remote NWK leave request
command frame received by the local device is accepted. */
#ifndef CS_NWK_LEAVE_REQ_ALLOWED
  #define CS_NWK_LEAVE_REQ_ALLOWED         true
#endif

/** \brief This flag determines schemes of permit joining in case of permit join value equal to 0xFF
  0 - infinite permittion;
  1 - three hours permittion;
  2 - 254 seconds permission */
#ifndef CS_NWK_MAX_PERMIT_JOIN_PERIOD_LOGIC
  #define CS_NWK_MAX_PERMIT_JOIN_PERIOD_LOGIC 0U
#endif

/** \brief A source node of a route discovery should be notified by
 * an intermediate node if its link cost to destination node is greater than
 * given parameter.
 * <b>Value range:</b> 0 - 8 \n
 */
#ifndef CS_SOFT_LINK_COST_THRESHOLD
  #define CS_SOFT_LINK_COST_THRESHOLD      8U
#endif /* CS_SOFT_LINK_COST_THRESHOLD */

/******************************************************************************
                    Functions prototypes section
******************************************************************************/

/**************************************************************************//**
\brief Set Configuration Server memory to its default state
******************************************************************************/
void csSetToDefault(void);

#endif  // _CSDEFAULTS_H
/* eof cdDefaults.h*/
