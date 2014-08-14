/***************************************************************************//**
  \file zclBasicCluster.h

  \brief
    The header file describes the ZCL Basic Cluster and its interface

    The file describes the types and interface of the ZCL Basic Cluster

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2014, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    03.01.09 A. Potashov - Created.
*******************************************************************************/

#ifndef _ZCLBASICCLUSTER_H
#define _ZCLBASICCLUSTER_H

/*!
Attributes and commands for determining basic information about a device,
setting user device information such as location, enabling a device and resetting it
to factory defaults.
*/

/*******************************************************************************
                   Includes section
*******************************************************************************/

#include <zcl.h>
#include <clusters.h>

/*******************************************************************************
                   Define(s) section
*******************************************************************************/

/***************************************************************************//**
  \brief ZCL Basic Cluster server side commands amount
*******************************************************************************/
#define ZCL_BASIC_CLUSTER_SERVER_ATTRIBUTES_AMOUNT        3
//The ZCLVersion and PowerSource attributes

/***************************************************************************//**
  \brief ZCL Basic Cluster derver side commands amount
*******************************************************************************/
#define ZCL_BASIC_CLUSTER_SERVER_COMMANDS_AMOUNT          0
//There are no any commands at the server side

/***************************************************************************//**
  \brief ZCL Basic Cluster client side commands amount
*******************************************************************************/
#define ZCL_BASIC_CLUSTER_CLIENT_ATTRIBUTES_AMOUNT        0
//There are no any attributes at the client side

/***************************************************************************//**
  \brief ZCL Basic Cluster derver side commands amount
*******************************************************************************/
#define ZCL_BASIC_CLUSTER_CLIENT_COMMANDS_AMOUNT          0
//There are no any commands at the client side


//Atribute Ids of Basic Device Information Attribute Set at the server side
//!ZCL Basic Cluster server side ZCLVersion attribute id
#define ZCL_BASIC_CLUSTER_SERVER_ZCL_VERSION_ATTRIBUTE_ID               CCPU_TO_LE16(0x0000)
//!ZCL Basic Cluster server side ApplicationVersion attribute id
#define ZCL_BASIC_CLUSTER_SERVER_APPLICATION_VERSION_ATTRIBUTE_ID       CCPU_TO_LE16(0x0001)
//!ZCL Basic Cluster server side StackVersion attribute id
#define ZCL_BASIC_CLUSTER_SERVER_STACK_VERSION_ATTRIBUTE_ID             CCPU_TO_LE16(0x0002)
//!ZCL Basic Cluster server side HWVersion attribute id
#define ZCL_BASIC_CLUSTER_SERVER_HW_VERSION_ATTRIBUTE_ID                CCPU_TO_LE16(0x0003)
//!ZCL Basic Cluster server side ManufacturerName attribute id
#define ZCL_BASIC_CLUSTER_SERVER_MANUFACTURER_NAME_ATTRIBUTE_ID         CCPU_TO_LE16(0x0004)
//!ZCL Basic Cluster server side ModelIdentifier attribute id
#define ZCL_BASIC_CLUSTER_SERVER_MODEL_IDENTIFIER_ATTRIBUTE_ID          CCPU_TO_LE16(0x0005)
//!ZCL Basic Cluster server side DateCode attribute id
#define ZCL_BASIC_CLUSTER_SERVER_DATE_CODE_ATTRIBUTE_ID                 CCPU_TO_LE16(0x0006)
//!ZCL Basic Cluster server side PowerSource attribute id
#define ZCL_BASIC_CLUSTER_SERVER_POWER_SOURCE_ATTRIBUTE_ID              CCPU_TO_LE16(0x0007)

//Atribute Ids of Basic Device Settings Attribute Set at the server side
//!ZCL Basic Cluster server side LocationDescription attribute id
#define ZCL_BASIC_CLUSTER_SERVER_LOCATION_DESCRIPTION_ATTRIBUTE_ID      CCPU_TO_LE16(0x0010)
//!ZCL Basic Cluster server side PhysicalEnvironment attribute id
#define ZCL_BASIC_CLUSTER_SERVER_PHYSICAL_ENVIRONMENT_ATTRIBUTE_ID      CCPU_TO_LE16(0x0011)
//!ZCL Basic Cluster server side DeviceEnabled attribute id
#define ZCL_BASIC_CLUSTER_SERVER_DEVICE_ENABLED_ATTRIBUTE_ID            CCPU_TO_LE16(0x0012)
//!ZCL Basic Cluster server side AlarmMask attribute id
#define ZCL_BASIC_CLUSTER_SERVER_ALARM_MASK_ATTRIBUTE_ID                CCPU_TO_LE16(0x0013)

//!ZCL Basic Cluster server side ResetToFactoryDeafaults commabd id
#define ZCL_BASIC_CLUSTER_SERVER_RESET_TO_FACTORY_DEFAULTS_COMMAND_ID   0x00

/*******************************************************************************
                   Types section
*******************************************************************************/

/***************************************************************************//**
  \brief ZCL Basic Cluster server side attributes defining macros

  This macros should be used for ZCL Basic Cluster server side attributes defining.

  \return None

  \internal
  //The typical usage is:
  //Basic Cluster server side related attributes
  ZCL_BasicClusterServerAttributes_t basicClusterAttributes = ZCL_DEFINE_BASIC_CLUSTER_SERVER_ATTRIBUTES();
*******************************************************************************/
#define ZCL_DEFINE_BASIC_CLUSTER_SERVER_ATTRIBUTES()                                                            \
  DEFINE_ATTRIBUTE(zclVersion, ZCL_READONLY_ATTRIBUTE, ZCL_BASIC_CLUSTER_SERVER_ZCL_VERSION_ATTRIBUTE_ID,  ZCL_U8BIT_DATA_TYPE_ID),    \
  DEFINE_ATTRIBUTE(powerSource, ZCL_READONLY_ATTRIBUTE, ZCL_BASIC_CLUSTER_SERVER_POWER_SOURCE_ATTRIBUTE_ID, ZCL_8BIT_ENUM_DATA_TYPE_ID),\
  DEFINE_ATTRIBUTE(physicalEnvironment, ZCL_READWRITE_ATTRIBUTE, ZCL_BASIC_CLUSTER_SERVER_PHYSICAL_ENVIRONMENT_ATTRIBUTE_ID, ZCL_8BIT_ENUM_DATA_TYPE_ID)


/***************************************************************************//**
  \brief ZCL Basic Cluster server side defining macros

  This macros should be used with #ZCL_DEFINE_BASIC_CLUSTER_SERVER_ATTRIBUTES for
  ZCL Basic Cluster server side defining in application.

  \param attributes - pointer to cluster server attributes (ZCL_BasicClusterServerAttributes_t)

  \return None

  \internal
  //The typical code is:
  //Basic Cluster server side related attributes
  ZCL_BasicClusterServerAttributes_t basicClusterServerAttributes = DEFINE_ZCL_BASIC_CLUSTER_SERVER_ATTRIBUTES();
  ZCL_Cluster_t myClusters[] =
  {
    ZCL_DEFINE_BASIC_CLUSTER_SERVER(&basicClusterServerAttributes),
    //... Any other cluster defining ...
  }
*******************************************************************************/
#define ZCL_DEFINE_BASIC_CLUSTER_SERVER(clattributes)                                                            \
  {                                                                                                              \
    .id = BASIC_CLUSTER_ID,                                                                                      \
    .options = {.type = ZCL_SERVER_CLUSTER_TYPE, .security = ZCL_NETWORK_KEY_CLUSTER_SECURITY},                  \
    .attributesAmount = ZCL_BASIC_CLUSTER_SERVER_ATTRIBUTES_AMOUNT,                                              \
    .attributes = (uint8_t *) clattributes,                                                                      \
    .commandsAmount = ZCL_BASIC_CLUSTER_SERVER_COMMANDS_AMOUNT,                                                  \
    .commands = NULL                                                                                             \
  }

#define ZCL_DEFINE_BASIC_CLUSTER_SERVER_FLASH(clattributes) ZCL_DEFINE_BASIC_CLUSTER_SERVER(clattributes)
/***************************************************************************//**
  \brief ZCL Basic Cluster client side defining macros

  This macros should be used for ZCL Basic Cluster client side defining in application.

  \return None

  \internal
  //The typical code is:
  ZCL_Cluster_t myClusters[] =
  {
    ZCL_DEFINE_BASIC_CLUSTER_CLIENT(),
    //... Any other cluster defining ...
  }
*******************************************************************************/
#define ZCL_DEFINE_BASIC_CLUSTER_CLIENT()                                                                        \
  {                                                                                                              \
    .id = BASIC_CLUSTER_ID,                                                                                      \
    .options = {.type = ZCL_CLIENT_CLUSTER_TYPE, .security = ZCL_NETWORK_KEY_CLUSTER_SECURITY},                  \
    .attributesAmount = ZCL_BASIC_CLUSTER_CLIENT_ATTRIBUTES_AMOUNT,                                              \
    .attributes = NULL,                                                                                          \
    .commandsAmount = ZCL_BASIC_CLUSTER_CLIENT_COMMANDS_AMOUNT,                                                  \
    .commands = NULL                                                                                             \
  }

#define ZCL_DEFINE_BASIC_CLUSTER_CLIENT_FLASH() ZCL_DEFINE_BASIC_CLUSTER_CLIENT()
BEGIN_PACK
/***************************************************************************//**
  \brief
    ZCL Basic Cluster server side attributes
*******************************************************************************/
typedef struct PACK
{
  //!Basic Device Information attribute set (Id == 0x00)

  /*!ZCL Version attribute (Read only, M)
  The ZCLVersion attribute is 8-bits in length and specifies the version number of
  the ZigBee Cluster Library that all clusters on this endpoint conform to. For the
  initial version of the ZCL, this attribute shall be set to 0x01.
  */
  struct PACK
  {
    ZCL_AttributeId_t   id;           //!<Attribute Id (0x0000, #ZCL_BASIC_CLUSTER_SERVER_ZCL_VERSION_ATTRIBUTE_ID)
    uint8_t             type;         //!<Attribute type (unsigned 8-bit integer, #ZCL_U8BIT_DATA_TYPE_ID)
    uint8_t             properties;   //!<Attribute properties bitmask
    uint8_t             value;        //!<Attribute value (default 0x00)
  } zclVersion;

  /*!Power Source attribute (Read only, M)
  The PowerSource attribute is 8-bits in length and specifies the source(s) of power
  available to the device. Bits b0 of this attribute represent the primary power
  source of the device and bit b7 indicates whether the device has a secondary power
  source in the form of a battery backup.

  Bits b0 of this attribute shall be set to one of the non-reserved values:
  0x00        - Unknown
  0x01        - Mains (single phase)
  0x02        - Mains (3 phase)
  0x03        - Battery
  0x04        - DC source
  0x05        - Emergency mains constantly powered
  0x06        - Emergency mains and transfer switch
  0x07-0x7f   - Reserved

  Bit b7 of this attribute shall be set to 1 if the device has a secondary power source
  in the form of a battery backup. Otherwise, bit b7 shall be set to 0.
  */
  struct PACK
  {
    ZCL_AttributeId_t   id;           //!<Attribute Id (0x0007, #ZCL_BASIC_CLUSTER_SERVER_POWER_SOURCE_ATTRIBUTE_ID)
    uint8_t             type;         //!<Attribute type (8-bit enumeration, #ZCL_8BIT_ENUM_DATA_TYPE_ID)
    uint8_t             properties;   //!<Attribute properties bitmask
    uint8_t             value;        //!<Attribute value (default 0x00)
  } powerSource;

  /*!Physical Environment attribute (Read only, O)
  The PhysicalEnvironment attribute is 8-bits in length and specifies the type of
  physical environment in which the device will operate.
  */
  struct PACK
  {
    ZCL_AttributeId_t   id;           //!<Attribute Id (0x0011, #ZCL_BASIC_CLUSTER_SERVER_PHYSICAL_ENVIRONMENT_ATTRIBUTE_ID)
    uint8_t             type;         //!<Attribute type (8-bit enumeration, #ZCL_8BIT_ENUM_DATA_TYPE_ID)
    uint8_t             properties;   //!<Attribute properties bitmask
    uint8_t             value;        //!<Attribute value (default 0x00)
  } physicalEnvironment;

  //!Basic Device Settings attribute set (Id == 0x01)
} ZCL_BasicClusterServerAttributes_t;

END_PACK

#if 0
// The ResetToFactoryDefaults command is optional now.
/***************************************************************************//**
  \brief
    Describes the Basic Cluster server side commands

    On receipt of this command, the device resets all the attributes of all its clusters to
  their factory defaults.
    Note that ZigBee networking functionality and any bindings are not affected by
  this command.
    This command does not have a payload.
*******************************************************************************/
typedef struct PACK
{
  struct
  {
    ZCL_CommandId_t       id;       //!<Command Id (0x00, #ZCL_BASIC_CLUSTER_SERVER_RESET_TO_FACTORY_DEFAULTS_COMMAND_ID)
    ZclCommandOptions_t   options;  //!<Command options (.direction should be equal to #ZCL_FRAME_CONTROL_DIRECTION_CLIENT_TO_SERVER)
    /*!
      \brief
        Command indication handler
      \param payloadLength - actual payload length in bytes (should be zero)
    */
    void (*resetToFactoryDefaults)(uint8_t payloadLength, uint8_t *payload);
  } resetToFactoryDefaults;
} BasicClusterServerCommands_t;
#endif //#if 0


#endif //#ifndef _ZCLBASICCLUSTER_H

//eof zclBasicCluster.h
