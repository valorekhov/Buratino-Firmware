/**************************************************************************//**
  \file sysEvents.h

  \brief List of events for Bitcloud(tm) events facility

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2014, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2012-02-28 D. Loskutnikov - Created
    2012-06-20 Max Gekk - The permissions request event and actions are added.
 ******************************************************************************/
#if !defined _SYS_EVENTS_H
#define _SYS_EVENTS_H

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <sysEventsHandler.h>

/******************************************************************************
                              Definitions section
 ******************************************************************************/
/** The list of stack actions for which it is necessary to request permissions. */
/** Receiving of the Match_Desc_req (ClusterId = 0x0006). See ZigBee spec. r19,
 * 2.4.3.1.7, page 106. The context field contains pointer to the APS_DataInd_t
 * structure of the Match_Desc_req. */
#define BC_MATCH_DESCR_REQ_ACTION      (1U << 0)
/** Receiving of the Bind_req (ClusterId = 0x0021). See ZigBee spec. r19,
 * 2.4.3.2.2, page 126. The context field contains pointer to the APS_DataInd_t
 * structure of the Bind_req. */
#define BC_BIND_REQ_ACTION             (1U << 1)
/** Receiving of the Unbind_req (ClusterId = 0x0022). See ZigBee spec. r19,
 * 2.4.3.2.3, page 128. The context field contains pointer to the APS_DataInd_t
 * structure of the Unbind_req. */
#define BC_UNBIND_REQ_ACTION           (1U << 2)
/** Receiving and handling of the APS Update Device command. See ZigBee spec. r19,
 * 4.4.9.3, page 481. The context field contains pointer to the aligned extended
 * address of an joined device. */
#define BC_UPDATE_DEVICE_REQ_ACTION    (1U << 3)
/** Received beacon passed for custom filtering. The context field contains pointer
 *  to the MAC_BeaconNotifyInd_t structure */
#define BC_BEACON_FILTERING_ACTION     (1U << 4)
/** Child request for address allocation during the join procedure appeared.
 *  The context field keeps a proposed NWK address */
#define BC_CHILD_NWK_ADDR_ALLOC_ACTION  (1U << 5)
/** Received frame with ProfileId different from one, used by destination endpoint.
 *  The context field keeps a structure of pointer to destination endpoint and conflicting
 *  ProfileId  */
#define BC_FRAME_FILTERING_BY_PROFILE_ID_ACTION (1U << 6)
/** Parent poll procedure failed. The context field keeps an amount of consequential
 *  fails. */
#define BC_POLL_FAILED_ACTION                   (1U << 7)

/******************************************************************************
                                Types section
 ******************************************************************************/
/*! \brief Identifiers of the evetns used by the stack. The SYS_SubscribeToEvent()
function may be used to add handler functions for these events.

An event may be provided with context information known as the event's data.
Event's data is provided in the second argument of a callback function
subscribed to the event (see SYS_EventReceiver_t::func). The argument is of
the SYS_EventData_t type. If event's data is one- or two-bytes long it is
copied into the argument. Otherwise, the argument contains the pointer to the type
holding event's data and should be casted to this pointer type.
*/
typedef enum _BcEvents_t
{
  /*! Connection to the network has been lost. Event's data is null. */
  BC_EVENT_NETWORK_LOST      = 0x00,
  /*! Connection to the trust center has been lost. Event's data is null. */
  BC_EVENT_TC_OFFLINE        = 0x01,
  /*! A new child has joined the network through the own node. Event's
      data: ChildInfo_t; provides information about the new node. */
  BC_EVENT_CHILD_JOINED      = 0x02,
  /*! A child node has left the network. Event's data: ChildInfo_t; only the
      ChildInfo_t::extAddr field is set - with the child's extended address.  */
  BC_EVENT_CHILD_REMOVED     = 0x03,
  /*! Network has been left by the own node. Event's data is null. */
  BC_EVENT_NETWORK_LEFT      = 0x04,
  /*! Network has been rejoined. Event's data is null. */
  BC_EVENT_NETWORK_STARTED   = 0x05,
  /*! Some network parameters (network address, PANID, or the working channel)
      have been changed. Event's data is null. */
  BC_EVENT_NETWORK_UPDATE    = 0x06,
  /*! Security keys for the joining device have not been found. Event's
      data: ChildInfo_t; provides information about the joining device. */
  BC_EVENT_CHILD_KEY_ABSENT  = 0x07,
  /*! A conflict of static short addresses has occured. Event's data: ShortAddr_t; -----
      contains the conflicting address. */
  BC_EVENT_ADDRESS_CONFLICT  = 0x08,
  /*! An APS key-pair set's entry has been removed. Event's data: ExtAddr_t;
      contains the extended address from the removed entry. */
  BC_EVENT_KEY_PAIR_DELETED  = 0x09,
  /*! A new concentrator has been discovered (many-to-one routing). ----
      Event's data: NwkRoutingEntryIndex_t - an index in the routing table. */
  BC_EVENT_NEW_CONCENTRATOR  = 0x0A,
  /*! A device announcement has been received. Event's data: ExtAddr_t; the
      address of the announced node. */
  BC_EVENT_DEVICE_ANNCE      = 0x0B,
  /*! The link key has been updated as a result of key establishment.
      Event's data: ExtAddr_t; the extended address of the device which link
      key has been updated. */
  BC_EVENT_UPDATE_LINK_KEY   = 0x0C,
  /*! The event is raised on the trust center when a remote device leaves the
      network. Event's data: ExtAddr_t; the extended address of the device that
      leaves the network. */
  BC_EVENT_DEVICE_LEFT       = 0x0D,
  /*! The node has started or joined the network. Event's data is null. */
  BC_EVENT_NETWORK_ENTERED   = 0x0E,
  /*! An operation of writing to persistent memory performed by PDS has completed.
      Event's data is null. */
  BC_EVENT_STORING_FINISHED  = 0x0F,
  /*! An entry has been added to or removed from the APS binding table.
      Event's data is null. */
  BC_EVENT_BIND_TABLE_UPDATED          = 0x10,
  /*! A group has been added to or removed from the NWK group table. ----
      Event's data: NWK_GroupAddr_t; the group's address. */
  BC_EVENT_GROUP_TABLE_UPDATED         = 0x11,
  /*! A group has been removed from the NWK group table. Event's data is null. */
  BC_EVENT_GROUPS_REMOVED              = 0x12,
  /*! Permissions for a given node have been changed. Event's data: ExtAddr_t;
      the extended address of the affected node. */
  BC_EVENT_PERMISSION_TABLE_UPDATED    = 0x13,
  /*! NWK security table (holding network keys) has been updated. ------
      Event's data: NwkKeyIndex_t; the index of the updated network key entry. */
  BC_EVENT_NWK_SECURITY_TABLES_UPDATED = 0x14,
  /*! APS key-pair set has been updated.
      Event's data: APS_KeyHandle_t; the handle of the updated entry. */
  BC_EVENT_APS_SECURITY_TABLES_UPDATED = 0x15,

  /*! A response for a match descriptor request sent during key establishment to
      discover devices supporting the KE cluster has been received.
      Event's data: ZDO_ZdpResp_t; the match descriptor response data. */
  BC_EVENT_KE_CLUSTER_MATCH_DESC_RESP  = 0x16,
  /*! The initiate key establishment request command has been sent. Event's data:
      ZCL_InitiateKeyEstablishmentCommand_t; the command's payload. */
  BC_EVENT_KE_CLUSTER_INITIATE_KE_REQ  = 0x17,
  /*! The initiate key establishment response command has been sent. Event's data:
      ZCL_InitiateKeyEstablishmentCommand_t; the command's payload. */
  BC_EVENT_KE_CLUSTER_INITIATE_KE_RESP = 0x18,
  /*! The ephemeral data request command has been received.
      Event's data: ZCL_EphemeralDataCommand_t; the command's payload. */
  BC_EVENT_KE_CLUSTER_EPH_DATA_REQ     = 0x19,
  /*! The ephemeral data response command has been received.
      Event's data: ZCL_EphemeralDataCommand_t; the command's payload. */
  BC_EVENT_KE_CLUSTER_EPH_DATA_RESP    = 0x1A,
  /*! The key data request command has been received.
      Event's data: ZCL_ConfirmKeyCommand_t; the command's payload. */
  BC_EVENT_KE_CLUSTER_CONF_KEY_REQ     = 0x1B,
  /*! The key data response command has been received.
      Event's data: ZCL_ConfirmKeyCommand_t; the command's payload. */
  BC_EVENT_KE_CLUSTER_CONF_KEY_RESP    = 0x1C,
  /*! The KE terminate command has been received. Event's data: ZCL_TKEStatus_t;
      the status code received with the command. */
  BC_EVENT_KE_CLUSTER_TERMINATE_CMD    = 0x1D,
  /*! A request for which persmissions may be set has been received.
      Event's data: BcAccessReq_t; the context differs depending on the
      request's type. */
  BC_EVENT_ACCESS_REQUEST              = 0x1E,
  /*! NWK security counters have to be saved to PDS */
  BC_SECURITY_NWK_COUNTERS_UPDATE      = 0x1F,
  /*! APS security counters have to be saved to PDS */
  BC_SECURITY_APS_COUNTERS_UPDATE      = 0x20,
  /*! ZDP command is received. */
  BC_EVENT_RX_ZDP_COMMAND              = 0x21,
  /*! Event on NWK updateId changing */
  BC_EVENT_NWK_UPDATEID_CHANGED        = 0x22,
  /*! Event on TC mode changing */
  BC_EVENT_TC_MODE_CHANGED             = 0x23,
  /*! Event on NWK channel changing */
  BC_EVENT_CHANNEL_CHANGED             = 0x24,
  /*! The stack raises this event when it tries to go to sleep mode.
      Any layers or application, that is subscribed to this event, may set passed data to false
      to prevent stack from going to sleep mode.
      Event's data: bool. */
  BC_EVENT_BUSY_REQUEST                = 0x25,
  /*! Top part of route request id is updated. */
  BC_EVENT_NWK_RREQ_ID_UPDATED         = 0x26,
  /*! ZDP request was received */
  BC_ZDP_REQUEST_RECEIVED              = 0x27,
  /*! ZDP response was received */
  BC_ZDP_RESPONSE_RECEIVED             = 0x28,
  /*! ZDP message was received */
  BC_ZDP_DEVICE_ANNOUNCE_RECEIVED      = 0x29,
  /*! ZDP device annce was received */
  BC_ZDP_JOIN_PERMITTED_CHANGED        = 0x2A,
  /*! Task processing completed */
  SYS_EVENT_TASK_PROCESSED             = 0x2B,
  /*! Device is entering sleep mode */
  HAL_EVENT_FALLING_ASLEEP             = 0x2C,
  /*! Device is waking up from sleep mode */
  HAL_EVENT_WAKING_UP                  = 0x2D,
  /*! A request for maximum time that device can sleep before active
      state is required */
  BC_EVENT_MAX_SLEEP_TIME_REQUEST      = 0x2E,
  /*! Device woke up for timer reload and is continuing sleep mode */
  HAL_EVENT_CONTINUING_SLEEP           = 0x2F,
  /*! Secured parameter was modified inside the NV Storage */
  BC_EVENT_NV_MEMORY_SECURITY_COMPROMISED = 0x30,
  /*! Parent link failure has been detected. */
  BC_EVENT_PARENT_LINK_FAILURE         = 0x31,
  /*! Connection to ZSI device on over side has been lost. */
  BC_ZSI_LOST_SYNCHRONIZATION          = 0x32,
  /*! A conflict of incoming frame counters has occured.
      Event's data: ShortAddr_t; - contains the address device with failed counter. */
  BC_EVENT_BAD_FRAME_COUNTER           = 0x33,
  /*! Remove Device is received from TC */
  BC_EVENT_REMOVE_DEVICE_RECEIVED      = 0x34,
  /*! Leave command is received from parent */
  BC_EVENT_LEAVE_COMMAND_RECEIVED      = 0x35,
  /*! Couldn't obtain security material for incoming frame.
      Event's data: ShortAddr_t; - contains the address device with failed counter. */
  BC_EVENT_FRAME_SECURITY_FAILED       = 0x36,
  /*! Device is in network already, allow address conflict resolving */
  BC_EVENT_ADDRESS_CONFLICT_RESOLVING_ALLOWED = 0x37,
  /*! This event is raised only if SLEEP_WHEN_IDLE feature is enabled.
      The stack raises this event when it checks whether polling should be turned on or not for end device.
      Any layers or application, that is subscribed to this event, may set passed data to true
      if polling is required. That allows to make an end device sleep when it wait fir some response from its parent
      and wake up every CS_POLL_PERIOD ms to poll its parent.
      Event's data: bool. */
  BC_EVENT_POLL_REQUEST                = 0x38,
} BcEvents_t;

/** Type of an external action. For example, receiving of a Mgmt-Bind.request
 * command. Actions can be combined using the OR operand. */
typedef uint16_t BcAction_t;

/** Type of data which is passed as the data parameter into an event handler
 * of the BC_EVENT_ACCESS_REQUEST event. An initiator must initialize all
 * fields of the structure before calling of the SYS_PostEvent() function. */
typedef struct _BcAccessReq_t
{
  /** An action for which permissions are requested. */
  BcAction_t action;
  /** The pointer to a parameter's structure which are associated with the action. */
  const void *context;
  /** The result of access request. The value 0 means the action is permitted,
   * any other values mean access denied. The initiator must write a default value. */
  uintptr_t denied;
} BcAccessReq_t;

#endif /* _SYS_EVENTS_H */
/** eof sysEvents.h */
