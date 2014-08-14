#ifndef _CONFIGURATION_H_
#define _CONFIGURATION_H_


#ifndef NONE_OS
#define NONE_OS
#endif
#ifndef _IEEE_ZIGBEE_COMPLIANCE_
#define _IEEE_ZIGBEE_COMPLIANCE_
#endif
#ifndef _SYS_MAC_PHY_HWD_TASK_
#define _SYS_MAC_PHY_HWD_TASK_
#endif
#ifndef _SYS_HAL_TASK_
#define _SYS_HAL_TASK_
#endif
#ifndef _SYS_MAC_HWI_TASK_
#define _SYS_MAC_HWI_TASK_
#endif
#ifndef _SYS_BSP_TASK_
#define _SYS_BSP_TASK_
#endif
#ifndef _SYS_APL_TASK_
#define _SYS_APL_TASK_
#endif
#ifndef _SYS_NWK_TASK_
#define _SYS_NWK_TASK_
#endif
#ifndef _SYS_APS_TASK_
#define _SYS_APS_TASK_
#endif
#ifndef _SYS_SSP_TASK_
#define _SYS_SSP_TASK_
#endif
#ifndef _SYS_TC_TASK_
#define _SYS_TC_TASK_
#endif
#ifndef _SYS_ZDO_TASK_
#define _SYS_ZDO_TASK_
#endif
#ifndef _SECURITY_
#define _SECURITY_
#endif
#ifndef _NWK_ALLOCATOR_
#define _NWK_ALLOCATOR_
#endif
#ifndef _HAL_HW_AES_
#define _HAL_HW_AES_
#endif
#ifndef _TRUST_CENTRE_
#define _TRUST_CENTRE_
#endif
#ifndef _DISTRIBUTED_TRUST_CENTER_
#define _DISTRIBUTED_TRUST_CENTER_
#endif
#ifndef _LINK_SECURITY_
#define _LINK_SECURITY_
#endif
#ifndef _COORDINATOR_
#define _COORDINATOR_
#endif
#ifndef _ROUTER_
#define _ROUTER_
#endif
#ifndef _ENDDEVICE_
#define _ENDDEVICE_
#endif
#ifndef _FFD_
#define _FFD_
#endif
#ifndef _MAC_BAN_NODE_
#define _MAC_BAN_NODE_
#endif
#ifndef _RF_REG_ACCESS_
#define _RF_REG_ACCESS_
#endif
#ifndef _CONTROL_FRAME_PENDING_BIT_
#define _CONTROL_FRAME_PENDING_BIT_
#endif
#ifndef _PENDING_EMPTY_DATA_FRAME_
#define _PENDING_EMPTY_DATA_FRAME_
#endif
#ifndef _NWK_FAST_ROUTE_DISCOVERY_
#define _NWK_FAST_ROUTE_DISCOVERY_
#endif
#ifndef _NWK_NONSTANDARD_BEACON_FILTER_
#define _NWK_NONSTANDARD_BEACON_FILTER_
#endif
#ifndef _NWK_GROUP_
#define _NWK_GROUP_
#endif
#ifndef _GROUP_TABLE_
#define _GROUP_TABLE_
#endif
#ifndef _NWK_CHECK_OUT_BROADCAST_
#define _NWK_CHECK_OUT_BROADCAST_
#endif
#ifndef _NWK_ROUTING_OPTIMIZATION_
#define _NWK_ROUTING_OPTIMIZATION_ 2
#endif
#ifndef _NWK_STOCHASTIC_ADDRESSING_
#define _NWK_STOCHASTIC_ADDRESSING_
#endif
#ifndef _RESOLVE_ADDR_CONFLICT_
#define _RESOLVE_ADDR_CONFLICT_
#endif
#ifndef _NWK_MESH_ROUTING_
#define _NWK_MESH_ROUTING_
#endif
#ifndef _APS_FRAGMENTATION_
#define _APS_FRAGMENTATION_
#endif
#ifndef _APS_MULTICAST_
#define _APS_MULTICAST_
#endif
#ifndef _GROUP_TABLE_
#define _GROUP_TABLE_
#endif
#ifndef _BINDING_
#define _BINDING_
#endif
#ifndef _COMMISSIONING_
#define _COMMISSIONING_
#endif
#ifndef _ENABLE_PERSISTENT_SERVER_
#define _ENABLE_PERSISTENT_SERVER_
#endif
#ifndef _SYS_PDS_TASK_
#define _SYS_PDS_TASK_
#endif
#ifndef _INTERPAN_
#define _INTERPAN_
#endif
#ifndef _NWK_ORPHAN_JOIN_
#define _NWK_ORPHAN_JOIN_
#endif
#ifndef _NWK_PASSIVE_ACK_
#define _NWK_PASSIVE_ACK_
#endif
#ifndef _CUSTOM_PASSIVE_ACK_THRESHOLD_
#define _CUSTOM_PASSIVE_ACK_THRESHOLD_
#endif
#ifndef _ZAPPSI_
#define _ZAPPSI_
#endif
#ifndef _SILENT_LEAVE_WITHOUT_NETWORK_LEFT_
#define _SILENT_LEAVE_WITHOUT_NETWORK_LEFT_
#endif
#ifndef _NWK_NEIGHBOR_ENTRY_ADDITION_UPON_RX_FRAME_
#define _NWK_NEIGHBOR_ENTRY_ADDITION_UPON_RX_FRAME_
#endif
#ifndef _NWK_ROUTE_REQUEST_RETRIES_BASED_ON_MAC_CONF_
#define _NWK_ROUTE_REQUEST_RETRIES_BASED_ON_MAC_CONF_
#endif
#ifndef _NWK_KEEP_STALE_NEIGHBOR_ENTRY_
#define _NWK_KEEP_STALE_NEIGHBOR_ENTRY_
#endif
#ifndef _LEAVE_NETWORK_IMMEDIATE_WITH_NO_TANSACTIONS_LEFT_
#define _LEAVE_NETWORK_IMMEDIATE_WITH_NO_TANSACTIONS_LEFT_
#endif
#ifndef _SLEEP_WHEN_IDLE_
#define _SLEEP_WHEN_IDLE_
#endif
#ifndef _NWK_IN_FRAME_COUNTERS_
#define _NWK_IN_FRAME_COUNTERS_
#endif
#ifndef _RF_BAT_MON_
#define _RF_BAT_MON_
#endif
#ifndef _SLEEP_WHEN_IDLE_
#define _SLEEP_WHEN_IDLE_
#endif


//-----------------------------------------------
// BSP identifiers
//-----------------------------------------------
#define BOARD_FAKE 0x00
#define BOARD_RCB 0x01
#define BOARD_RCB_KEY_REMOTE 0x02
#define BOARD_ATMEGA256RFR2_XPRO 0x03
#define BOARD_BURATINO 0x04

//-----------------------------------------------
// Includes board-specific peripherals support in application.
//-----------------------------------------------
#include <BoardConfig.h>

#undef _BUTTONS_

#define APP_INTERFACE_USART 0x01
#define APP_INTERFACE_VCP 0x02
#define APP_INTERFACE_SPI 0x03
#define APP_INTERFACE_UART 0x04
#define APP_INTERFACE_USBFIFO 0x05
#define APP_INTERFACE_STDIO 0x06

#define AT25F2048  0x01
#define AT45DB041  0x02
#define AT25DF041A 0x03

#define CONSOLE_ONLY 1
#define COMMON_LOGIC_ONLY 2
#define CONSOLE_AND_COMMON_LOGIC 3

// Enables or disables APS Fragmentation support.
#define APP_FRAGMENTATION 0
//#define APP_FRAGMENTATION 1

// Enables or disables support for OTA Upgrade.
#define APP_USE_OTAU 0
//#define APP_USE_OTAU 1

// Shall be enabled for HA application.
#define ZCL_SUPPORT 1

// Shall be enabled for HA application.
#define HA_PROFILE_EXTENSIONS 1

// ZigBee Home Automation profile device type
//#define APP_DEVICE_TYPE_COMBINED_INTERFACE
#define APP_DEVICE_TYPE_OCCUPANCY_SENSOR
//#define APP_DEVICE_TYPE_DIMMABLE_LIGHT
//#define APP_DEVICE_TYPE_DIMMER_SWITCH

#define APP_COMBINED_INTERFACE_EXT_ADDRESS 0x1LL

#define APP_DIMMABLE_LIGHT_EXT_ADDRESS 0x2LL

#define APP_DIMMER_SWITCH_EXT_ADDRESS 0x3LL

#define APP_OCCUPANCY_SENSOR_EXT_ADDRESS 0x4LL

// Specifies if logging enabled
//#define APP_DEVICE_EVENTS_LOGGING 1
#define APP_DEVICE_EVENTS_LOGGING 0

//#define APP_ENABLE_CONSOLE 1
#define APP_ENABLE_CONSOLE 0

/* Enable wear-leveling version of PDS */
#define PDS_ENABLE_WEAR_LEVELING 1

/* If Bootloader will be used in parallel with application. this should be commented. */
#define PDS_NO_BOOTLOADER_SUPPORT

/* ZigBee Platform NV items list*/
#define PERSISTENT_NV_ITEMS_PLATFORM    NWK_SECURITY_COUNTERS_MEM_ID
/* Application NV items list */
#define PERSISTENT_NV_ITEMS_APPLICATION 0xFFFu

//-----------------------------------------------
//BOARD_MEGARF
//-----------------------------------------------
#ifdef BOARD_MEGARF

  
  //#define BSP_SUPPORT BOARD_RCB
  #define BSP_SUPPORT BOARD_ATMEGA256RFR2_XPRO
  //#define BSP_SUPPORT BOARD_RCB_KEY_REMOTE
  //#define BSP_SUPPORT BOARD_FAKE
  
  // Defines primary serial interface type to be used by application.
  #define APP_INTERFACE APP_INTERFACE_USART
  //#define APP_INTERFACE APP_INTERFACE_USBFIFO
  
  //-----------------------------------------------
  //APP_INTERFACE == APP_INTERFACE_USART
  //-----------------------------------------------
  #if (APP_INTERFACE == APP_INTERFACE_USART)
	// Defines USART interface name to be used by application.
    #define APP_USART_CHANNEL USART_CHANNEL_1
    //#define APP_USART_CHANNEL USART_CHANNEL_0
  #endif
  
  //-----------------------------------------------
  //APP_INTERFACE == APP_INTERFACE_USBFIFO
  //-----------------------------------------------
  #if (APP_INTERFACE == APP_INTERFACE_USBFIFO)
    // Defines USART interface name to be used by application.
    #define APP_USART_CHANNEL USART_CHANNEL_USBFIFO
  #endif
  
  // Enable this option if target board supports controlling of TTL to RS232
  // converter enable pin.
  #define BSP_ENABLE_RS232_CONTROL 1
  //#define BSP_ENABLE_RS232_CONTROL 0
  
  // Defines primary serial interface type to be used by ZAppSI.
  #define APP_ZAPPSI_INTERFACE APP_INTERFACE_USART
  //#define APP_ZAPPSI_INTERFACE APP_INTERFACE_USBFIFO
  //#define APP_ZAPPSI_INTERFACE APP_INTERFACE_SPI
  
  //-----------------------------------------------
  //APP_ZAPPSI_INTERFACE == APP_INTERFACE_USART
  //-----------------------------------------------
  #if (APP_ZAPPSI_INTERFACE == APP_INTERFACE_USART)
    // Defines USART interface name to be used by ZAppSI.
    #define APP_ZAPPSI_MEDIUM_CHANNEL USART_CHANNEL_1
    //#define APP_ZAPPSI_MEDIUM_CHANNEL USART_CHANNEL_0
  #endif
  
  //-----------------------------------------------
  //APP_ZAPPSI_INTERFACE == APP_INTERFACE_USBFIFO
  //-----------------------------------------------
  #if (APP_ZAPPSI_INTERFACE == APP_INTERFACE_USBFIFO)
    // Defines USART interface name to be used by ZAppSI.
    #define APP_ZAPPSI_MEDIUM_CHANNEL USART_CHANNEL_USBFIFO
  #endif
  
  //-----------------------------------------------
  //APP_ZAPPSI_INTERFACE == APP_INTERFACE_SPI
  //-----------------------------------------------
  #if (APP_ZAPPSI_INTERFACE == APP_INTERFACE_SPI)
    // Defines USART interface name to be used by ZAppSI.
    #define APP_ZAPPSI_MEDIUM_CHANNEL SPI_CHANNEL_2
    
    // Defines SPI interface mode.
    #define APP_ZAPPSI_SPI_MASTER_MODE 0
  #endif
#endif //


// 32-bit mask of channels to be scanned before network is started. Channels that
// should be used are marked with logical 1 at corresponding bit location.
//  Valid channel numbers for 2.4 GHz band are 0x0b - 0x1a
//  Valid channel numbers for 900 MHz band are 0x00 - 0x0a
// 
//  Notes:
//  1. for small amount of enabled channels it is more convinient to specify list
// of channels in the form of '(1ul << 0x0b)'
//  2. For 900 MHz band you also need to specify channel page
// 
//  Value range: 32-bit values:
//  Valid channel numbers for 2.4 GHz band are 0x0b - 0x1a
//  Valid channel numbers for 900 MHz band are 0x00 - 0x0a
// 
//  C-type: uint32_t
//  Can be set: at any time before network start
#define CS_CHANNEL_MASK (1L<<0x0f) | (1L<<0x0b) | (1L<<0x0c) | (1L<<0x0d) | (1L<<0x0e) | (1L<<0x10) | (1L<<0x11) | (1L<<0x12) | (1L<<0x13) | (1L<<0x14) | (1L<<0x15) | (1L<<0x16) | (1L<<0x17) | (1L<<0x18) | (1L<<0x19) | (1L<<0x1A)

// The parameter specifies the predefined extended PANID of the network to be
// formed (for the coordinator) or joined (for a router or an end device). For a
// router or an end device the parameter can equal 0 allowing them to join the
// first suitable network that they discover.
#define CS_EXT_PANID 0x00LL

// The maximum number of direct children that a given device (the coordinator or a
// router) can have.
// 
//  The parameter is only enabled for routers and the coordinator. An end device
// can not have children. If an actual number of children reaches a parameter's
// value, the node will have not been able to accept any more children joining the
// network. The parameter can be set to 0 on a router thus preventing it from
// accepting any children and can help form a desired network topology. For
// example, if the parameter is set to 0 on all routers, then the coordinator will
// be the only device that can have children and the network will have star
// topology.
#define CS_MAX_CHILDREN_AMOUNT 6

// The maximum number of routers among the direct children of the device
// 
//  The parameter determines how many routers the device can have as children. Note
// that the maximum number of end devices is equal to CS_MAX_CHILDREN_AMOUNT -
// CS_MAX_CHILDREN_ROUTER_AMOUNT.
#define CS_MAX_CHILDREN_ROUTER_AMOUNT 2

// The parameter enabled in the high security mode specifies the size of the APS
// key-pair set. The APS key-pair set stores pairs of corresponding extended
// address and a link key or a master key. For each node with which the current
// node is going to communicate it must keep an entry with the remote node extended
// address and a link key. If the link key is unknown, the node can request the
// trust center for it via APS_RequestKeyReq(). The trust center must store a link
// key or a master key depending on the CS_SECURITY_STATUS used for each node it is
// going to authenticate. Entries can also be added manually by APS_SetLinkKey()
// and APS_SetMasterKey().
#define CS_APS_KEY_PAIR_DESCRIPTORS_AMOUNT 5

// The parameter is used to determine the security type.
// 
//  Value range: 0,3 - for standard security; 1,2 - for high security.
//  0 - network key is preconfigured ;
//  1 - network join without master key, but with a trust center link key, which
// must be set via APS_SetLinkKey();
//  2 - network join employs a master key, which must be set APS_SetMasterKey();
//  3 - network key is no preconfigured, but rather received from the trust center
// in an unencrypted frame. <br.
#define CS_ZDO_SECURITY_STATUS 1
//#define CS_ZDO_SECURITY_STATUS 2

// Depending on security key type and security mode this is either network key,
// master key, link key or initial link key.
// 
//  Network key is used to encrypt a part of a data frame occupied by the NWK
// payload. This type of encryption is applied in both the standard and high
// security mode. The high security mode also enables encryption of the APS payload
// with a link key, but if the txOptions.useNwkKey field in APS request parameters
// is set to 0, the APS payload is encrypted with the network key.
// 
//  The network key must be predefined if standard security is used with
// CS_ZDO_SECURITY_STATUS set to 0. For all other values of CS_ZDO_SECURITY_STATUS
// the network key is received from the trust center during device authentication.
// Note that in the standard security mode with CS_ZDO_SECURITY_STATUS equal to 3
// the network key is transferred to the joining device in an unencrypted frame.
#define CS_NETWORK_KEY {0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC}

// The maximum number of network keys that can be stored on the device
// 
//  A device in a secured network can keep several network keys up to the value of
// this parameter. Upon frame reception the device extracts key sequence number
// from the auxiliary header of the frame and decrypts the message with the network
// key corresponding to this sequence number. Besides, one key is considered active
// for each device; this is the key that is used for encrypting outgoing frames.
// The keys are distributed by the trust center with the help of the
// APS_TransportKeyReq() command. The trust center can also change the active key
// of a remote node via a call to APS_SwitchKeyReq().
#define CS_NWK_SECURITY_KEYS_AMOUNT 1

// Security information waiting timeout before secure network join considered
// failed.
// 
//  A timeout is started when connection with a parent is established. If the
// security related procedures that are performed after this will not be completed
// before the timeout exceeds, the device will fail joining the network. A value is
// measured in milliseconds.
#define CS_APS_SECURITY_TIMEOUT_PERIOD 10000

// Maximum amount of records in the Group Table.
// 
//  The Group Table size cannot be 0. The group table stores pairs of a group
// address and an endpoint. Upon receiving a frame addressed to members of a
// certain group which include the current node as well the stack fires indications
// on all endpoints registered with the group address.
#define CS_GROUP_TABLE_SIZE 8

// Maximum amount of records in the Neighbor Table.
// 
//  The parameter determines the size of the neighbor table which is used to store
// beacon responses from nearby devices. The parameter puts an upper bound over the
// amount of child devices possible for the node.
#define CS_NEIB_TABLE_SIZE 5

// Maximum amount of records in the network Route Table.
// 
//  The parameter sets the maximum number of records that can be kept in the NWK
// route table. The table is used by NWK to store information about established
// routes. Each table entry specifies the next-hop short address for a route from
// the current node to a given destination node. The table is being filled
// automatically during route discovery. An entry is added when a route is
// discovered.
#define CS_ROUTE_TABLE_SIZE 5

// Maximum amount of records in the network Address Map Table.
// 
//  The parameter sets the maximum number of records in the address map table used
// by NWK to store pairs of corresponding short and extended addresses. The stack
// appeals to the table when a data frame is being sent to a specified extended
// address to extract the corresponding short address. If it fails to find the
// short address, an error is reported.
#define CS_ADDRESS_MAP_TABLE_SIZE 5

// Maximum amount of records in the network Route Discovery Table.
// 
//  The parameter specifies the size of the route discovery table used by NWK to
// store next-hop addresses of the nodes for routes that are not yet established.
// Upon exhausting the capacity of the table, the stack starts rewriting old
// entries. If the size of the route table is big enough after all used routes are
// established the table may not be used.
#define CS_ROUTE_DISCOVERY_TABLE_SIZE 2

// Maximum amount of records in the Duplicate Rejection Table.
// 
//  The duplicate rejection table is used by APS to store information about
// incoming unicast messages in order to reject messages that have been already
// received and processed. Following ZigBee specification, the parameter should be
// not less than 1.
#define CS_DUPLICATE_REJECTION_TABLE_SIZE 7

// Maximim amount of records in the Binding Table
// 
//  The parameter sets the size of the binding table used by APS to store binding
// links, which are structures containing information about source and destination
// extended addresses and endpoints for unicast bindings and just group addresses
// as destinations for group bindings. If the binding is going to be applied to
// send a data frame, then the corresponding entry shall be first inserted into the
// table via the APS_BindingReq() function.
#define CS_APS_BINDING_TABLE_SIZE 10

// The number of buffers for data requests on the APS layer.
// 
//  The parameter specifies the number of buffers that are allocated by APS to
// store data requests parameters. The parameter puts an upper bound to the number
// of data requests that can be processed by APS simultaneously. If all buffers are
// in use and a new data request appears, it is kept in a queue until a buffer is
// released.
#define CS_APS_DATA_REQ_BUFFERS_AMOUNT 4

// The number of buffers for acknowledgement messages sent by APS.
// 
//  This parameter determines the amount of memory that needs to be allocated for a
// special type of buffers used by APS to store payloads for acknowledgement
// frames. The need to use the buffers occurs when the node receives a frame that
// has to be acknowledged. That is, the APS component on the node has to send an
// acknowledgement frame. For frames initiated by the application, the memory for a
// payload is to be allocated by the application on its own, while the payload
// memory for an acknowledgement frame shall be reserved by APS. The request
// parameters are still stored in the data request buffers.
#define CS_APS_ACK_FRAME_BUFFERS_AMOUNT 3

// Amount of buffers on NWK layer used to keep incoming and outgoing frames. This
// parameters affects how many children of a parent are able to get broadcat
// messages.
#define CS_NWK_BUFFERS_AMOUNT 8

// The parameter specifies the TX power of the transceiver device, is measured in
// dBm(s). After the node has entered the network the value can only be changed via
// the ZDO_SetTxPowerReq() function.
// 
//  Value range: depends on the hardware. Transmit power must be in the range from
// -17 to 3 dBm for AT86RF231, AT86RF230 and AT86RF230B. For AT86RF233 transmit
// power must be in the range from -17 to 4 dBm. For AT86RF212 transmit power must
// be in the range from -11 to 11 dBm.
#define CS_RF_TX_POWER 3

//-----------------------------------------------
//APP_FRAGMENTATION == 1
//-----------------------------------------------
#if (APP_FRAGMENTATION == 1)
  // This parameter limits the number of pieces to which the data sent with one APS
  // request can be split i f the fragmentation feature is applied. Thus it also
  // limits the maximum amount of data sent by the application with a single request:
  //  maximum data length = CS_APS_MAX_BLOCKS_AMOUNT * CS_APS_BLOCK_SIZE if the
  // latter parameter is not 0, else
  //  maximum data length = CS_APS_MAX_BLOCKS_AMOUNT * APS_MAX_ASDU_SIZE.
  #define CS_APS_MAX_BLOCKS_AMOUNT 4
  
  // Maximim size of a single fragment during fragmented transmission.
  // 
  //  If the value is 0, blocks will be of the maximum possilbe size; that is the
  // size will equal the value of CS_APS_MAX_ASDU_SIZE. The parameter and
  // CS_APS_MAX_BLOCKS_AMOUNT are also used to determine an amount of memory
  // allocated for a special buffer that keeps parts of incoming fragmented message
  // until all of them are received.
  #define CS_APS_BLOCK_SIZE 0
#endif

//-----------------------------------------------
//APP_USE_OTAU == 1
//-----------------------------------------------
#if (APP_USE_OTAU == 1)
  #undef ZCL_SUPPORT
  #define ZCL_SUPPORT 1
  
  // Enable or disable console tunneling support in image store driver. Application
  // can send date through tunnel in parallel to ISD driver.
  #define APP_USE_ISD_CONSOLE_TUNNELING 1
  //#define APP_USE_ISD_CONSOLE_TUNNELING 0
  
  // Use fake OFD driver instead of real one. This option is useful for evaluation of
  // OTA Upgrade feature on boards without external flash.
  #define APP_USE_FAKE_OFD_DRIVER 0
  //#define APP_USE_FAKE_OFD_DRIVER 1
  
  // Support OTAU image page request features
  #define APP_SUPPORT_OTAU_PAGE_REQUEST 1
  //#define APP_SUPPORT_OTAU_PAGE_REQUEST 0
  
  // Defines type of used external flash memory chip.
  #define EXTERNAL_MEMORY AT25DF041A
  //#define EXTERNAL_MEMORY AT25F2048
  //#define EXTERNAL_MEMORY AT45DB041
  
  // The amount of servers the OTAU client can listen to during upgrade server
  // discovery
  // 
  //  The OTAU client can store information about several discovered OTAU servers.
  // However, the client device tries to connect to discovered servers one by one
  // until the first successful attempt and then communicates and uploads the whole
  // image from the first suitable server.
  #define CS_ZCL_OTAU_DISCOVERED_SERVER_AMOUNT 1
  
  // The number of clients that the OTAU server can server simultaneously
  // 
  //  If this parameter equals 1, the OTAU server will upgrade devices in the network
  // one by one. However, the server can process more than one client sessions at a
  // time, if this parameter is greater than 1.
  #define CS_ZCL_OTAU_CLIENT_SESSION_AMOUNT 1
  
  // The interval in milliseconds between two attempts to find an upgrade server
  // 
  //  The parameter is valid for OTAU clients only.
  // 
  //  Value range: any 32-bit value
  //  C-type: uint32_t
  //  Can be set: at any time before an OTAU start
  #define CS_ZCL_OTAU_SERVER_DISCOVERY_PERIOD 60000
  
  // The default address of an upgrade server
  // 
  //  The parameter indicates how the OTAU client will search for OTAU servers in the
  // network. If one of broadcast addresses is specified, the client will attempt to
  // find all devices supporting the OTAU server cluster and will request new images
  // from the first server that will respond. Otherwise, the client will try to
  // connect to a particular device with the specified extended address.
  #define CS_ZCL_OTAU_DEFAULT_UPGRADE_SERVER_IEEE_ADDRESS 0xFFFFFFFFFFFFFFFFull
  
  // Indicates that image page request are used to load an image
  // 
  //  If the parameter is set to 1 the OTAU client will use image page requests
  // (rather than image block requests) to load an application image from the OTAU
  // server. In this case the server will send a whole page (consisting of multiple
  // blocks) without requesting an APS acknowledgement for each block. The client
  // marks not-delivered blocks and sends an image block request for each of missed
  // blocks after the transmission on the page finishes.
  #define CS_ZCL_OTAU_IMAGE_PAGE_REQUEST_ENABLE 1
  //#define CS_ZCL_OTAU_IMAGE_PAGE_REQUEST_ENABLE 0
#endif //

//-----------------------------------------------
//CS_ZCL_OTAU_IMAGE_PAGE_REQUEST_ENABLE == 1
//-----------------------------------------------
#if (CS_ZCL_OTAU_IMAGE_PAGE_REQUEST_ENABLE == 1)
  // Period in milliseconds between two image block responses sent from the OTAU
  // server
  // 
  //  The parameter indicates how fast the OTAU server shall send data (via the image
  // block response command) to the OTAU client. The value is set on the client. The
  // server receives the value from the client. The server shall wait for, at
  // minimum, the period specified in the parameter before sending another block to
  // the client.
  #define CS_ZCL_OTAU_IMAGE_PAGE_REQUEST_RESPONSE_SPACING 200
  
  // The page size - the number of bytes sent for a single image page request
  // 
  //  The parameter sets the number of bytes to be sent by the server for an image
  // page request sent by the OTAU client. A page is transmitted in several data
  // frames, one data frame for each block of data. The client, upon receiving
  // individual blocks, does not send an APS acknowledgement until all blocks for the
  // current page are not sent. Blocks that have not been delivered are requested by
  // the client via image block requests after loading of the current page finishes.
  #define CS_ZCL_OTAU_IMAGE_PAGE_REQUEST_PAGE_SIZE 256
#endif

//-----------------------------------------------
//APP_DEVICE_TYPE_COMBINED_INTERFACE
//-----------------------------------------------
#ifdef APP_DEVICE_TYPE_COMBINED_INTERFACE
  //-----------------------------------------------
  //APP_USE_OTAU == 1
  //-----------------------------------------------
  #if (APP_USE_OTAU == 1)
    // OTAU device role
    #define OTAU_SERVER
    //#define OTAU_CLIENT
  #endif
#else
  //-----------------------------------------------
  //APP_USE_OTAU == 1
  //-----------------------------------------------
  #if (APP_USE_OTAU == 1)
    // OTAU device role
    #define OTAU_CLIENT
    //#define OTAU_SERVER
  #endif
#endif


#include <appConsts.h>

#endif // _CONFIGURATION_H_
