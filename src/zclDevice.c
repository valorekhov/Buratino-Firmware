

/*******************************************************************************
                             Includes section
*******************************************************************************/
#include <apsConfigServer.h>
#include <zclDevice.h>
#include <uartManager.h>
#include <zclSecurityManager.h>
#include <pdsDataServer.h>
#include <identifyCluster.h>
#include <ezModeManager.H>
#include <sysTaskManager.h>
#include <bspUid.h>
#include <bspLeds.h>
#include <buratinoSettings.h>

#ifdef ZAPPSI_HOST
#include <zsiNotify.h>
#endif
#if defined ZAPPSI_HOST && defined WIN
#include <keyboardPoll.h>
#endif

/******************************************************************************
                    Definitions section
*******************************************************************************/
#define MAX_ATTEMPS_TO_FIND_CLUSTER  7
#define MAX_ATTEMPS_TO_ENTER_NETWORK 3

/******************************************************************************
                    Types section
******************************************************************************/
typedef enum
{
  APP_INITIAL_STATE,
  APP_START_WAIT_STATE,
  APP_START_NETWORK_STATE,
  APP_NETWORK_JOINING_STATE,
  APP_IN_NETWORK_STATE,
  APP_RESETTING_STACK_STATE,
} AppState_t;

/*******************************************************************************
                    Static functions section
*******************************************************************************/
static void ZDO_StartNetworkConf(ZDO_StartNetworkConf_t *conf);
static void initApp(void);
static void startNetwork(void);
static DeviceType_t detectNwkAddrAndDevType(void);
static void zdoResetNwkConf(ZDO_ResetNetworkConf_t *conf);
static void setDeviceJoinParameters(bool discoverNetworks, bool clearNeighborTable);
/*******************************************************************************
                    Global variables section
*******************************************************************************/
AppDeviceState_t appDeviceState = DEVICE_INITIAL_STATE;

/*******************************************************************************
                    Local variables section
*******************************************************************************/
static AppState_t appState = APP_INITIAL_STATE;
static ZDO_StartNetworkReq_t  zdoStartNetworkReq;
static uint8_t startNetworkAttempts;
static bool startBindingAndFinding = true;
static ZDO_ResetNetworkReq_t zdoResetNwkReq =
{
  .ZDO_ResetNetworkConf = zdoResetNwkConf
};
static bool restored;
static ChannelsMask_t channelMask;

/*******************************************************************************
                    Implementation section
*******************************************************************************/
/**************************************************************************//**
\brief Application task handler
******************************************************************************/
void APL_TaskHandler(void)
{
  switch (appState)
  {
    // node is in initial state
    case APP_INITIAL_STATE:                 // Initial (after RESET) state
      initApp();                            // Init application
      break;

    case APP_START_NETWORK_STATE:
      startNetwork();                       // Start/joing network
      appState = APP_NETWORK_JOINING_STATE;

    case APP_NETWORK_JOINING_STATE:         // Network is in the joining stage
      break;

    case APP_IN_NETWORK_STATE:              // Normal operating
      appDeviceTaskHandler();
      break;
    default:
      break;
  }
}

#ifdef ZAPPSI_HOST
/**************************************************************************//**
\brief Indicates zsi status update.

\param[out] notf - ZSI staus update indication parameters structure pointer. For details go to
            ZSI_UpdateNotf_t declaration.
\return none
******************************************************************************/
void ZSI_StatusUpdateNotf(ZSI_UpdateNotf_t *notf)
{
  (void)notf;
}

/**************************************************************************//**
\brief Indicates reset Application.

\param[out]
\return none
******************************************************************************/
void restartApp(void)
{
  appState = APP_INITIAL_STATE;
  appDeviceState = DEVICE_INITIAL_STATE;
  resetEzModeInProgress();
  idenityResetSubscribers();
  SYS_SysInit();
}
#endif // ZAPPSI_HOST

/**************************************************************************//**
\brief Network update notification

\param[in] nwkParams - update notification parameters
******************************************************************************/
void ZDO_MgmtNwkUpdateNotf(ZDO_MgmtNwkUpdateNotf_t *nwkParams)
{
  LOG_STRING(nwkUpdateStatusStr, "Network update status - 0x%02x\r\n");

  appSnprintf(nwkUpdateStatusStr, nwkParams->status);
  switch (nwkParams->status)
  {
    case ZDO_NETWORK_LEFT_STATUS:
    {
      // Make end device try to rejoin to any device of the same network it was before reset
      setDeviceJoinParameters(true, true);
      startNetworkAttempts = 0;

      identifyUpdateCommissioningState(false, false);
      appState = APP_START_NETWORK_STATE;

      SYS_PostTask(APL_TASK_ID);
    } break;

    default:
      break;
  }
}

/**************************************************************************//**
\brief End device wake up indication (required by stack)
******************************************************************************/
void ZDO_WakeUpInd(void)
{}

/**************************************************************************//**
\brief Restarts application

\param[in] startBinding - if true then device starts finding and binding procedure
******************************************************************************/
void appRestart(bool startBinding)
{
  if (APP_RESETTING_STACK_STATE != appState)
    appState = APP_RESETTING_STACK_STATE;
  else
    return;

  startBindingAndFinding = startBinding;

  appStop();
  ZDO_ResetNetworkReq(&zdoResetNwkReq);
}

/**************************************************************************//**
\brief Application and stack parameters initialization
******************************************************************************/
static void initApp(void)
{
  ExtAddr_t uid;
  //Reads the UID set in configuration.h
  CS_ReadParameter(CS_UID_ID,&uid);
  if (uid == 0 || uid > APS_MAX_UNICAST_EXT_ADDRESS)
  {
    BSP_ReadUid(&uid); //Will read the UID from chip
    CS_WriteParameter(CS_UID_ID, &uid); //Writes the read UID to the ram
  }

  DeviceType_t deviceType;

  CS_WriteParameter(CS_PERMIT_DURATION_ID, &(uint8_t){0});
  
  channelMask = (1L<<0x0b) | (1L<<0x0f) |  (1L<<0x0c) | (1L<<0x0d) | /*(1L<<0x0e) |*/ (1L<<0x10) | (1L<<0x11) | (1L<<0x12) | (1L<<0x13) | (1L<<0x14) | (1L<<0x15) | (1L<<0x16) | (1L<<0x17) | (1L<<0x18) | (1L<<0x19) | (1L<<0x1A);  
  CS_WriteParameter(CS_CHANNEL_MASK_ID, &channelMask);
  CS_ReadParameter(CS_CHANNEL_MASK_ID, &channelMask);

  appState = APP_START_WAIT_STATE;
  deviceType = detectNwkAddrAndDevType();
  // Set parameters to config server
  CS_WriteParameter(CS_DEVICE_TYPE_ID, &deviceType);
  if (APP_DEVICE_TYPE == DEV_TYPE_ENDDEVICE)
  {
    bool rxOnWhenIdle = false;
    CS_WriteParameter(CS_RX_ON_WHEN_IDLE_ID, &rxOnWhenIdle);
  }

  appState = APP_START_NETWORK_STATE;   // Application state to join network switching
  SYS_PostTask(APL_TASK_ID);

  initEzModeManager(appIsInitiator());
#ifndef ZAPPSI_HOST

  PDS_StoreByEvents(BC_ALL_MEMORY_MEM_ID);
  if (PDS_IsAbleToRestore(BC_ALL_MEMORY_MEM_ID) && PDS_Restore(BC_ALL_MEMORY_MEM_ID))
  {
    // Make end device try to rejoin to its parent on the same channel and to the same network it was before reset
    // Make router to do silent join while retaining the neighbor table entries
    setDeviceJoinParameters(false, false);
    restored = true;
    appDeviceInitAfterPowerFailure();
    return;
  } 
  /*
  extPanId: 16303394421632598365
  panId: 1577
  shortAddr: 53700
  parentAddr: 0
  channel: 15
  */
#endif
  appSecurityInit();
  appDeviceInit();
}

/**************************************************************************//**
\brief ZDO_StartNetwork primitive confirmation was received

\param[in] confirmInfo - confirmation information
******************************************************************************/
static void ZDO_StartNetworkConf(ZDO_StartNetworkConf_t *confInfo)
{
  SYS_PostTask(APL_TASK_ID);
  // Joined network successfully
  if (ZDO_SUCCESS_STATUS == confInfo->status)   // Network was started successfully
  {

    ExtPanId_t extPanId;
    /* Set the target extended PANID to be equal to the extended PANID of the joined PAN
     to ensure device won't join other networks. */
    CS_ReadParameter(CS_NWK_EXT_PANID_ID,&extPanId);
    CS_WriteParameter(CS_EXT_PANID_ID, &extPanId);
	
    CS_WriteParameter(CS_CHANNEL_MASK_ID, &channelMask);
    if (appDeviceState != DEVICE_ACTIVE_IDLE_STATE)
      appDeviceState = DEVICE_INITIAL_STATE;        // Set device state
    appState = APP_IN_NETWORK_STATE;

    identifyUpdateCommissioningState(true, false);

    if (!restored && startBindingAndFinding)
    {
      invokeEzMode(appEzModeDone);
      startBindingAndFinding = false;
    }// else if (restored){
		//appEzModeDone();
	//}	
	halOnSecondLed();
  }
  else
  {
    appState = APP_START_NETWORK_STATE;
    if (restored)
    {
      uint8_t currentChannel;

      CS_ReadParameter(CS_NWK_LOGICAL_CHANNEL_ID, &currentChannel);
      if (channelMask == (1ul << currentChannel))
        CS_WriteParameter(CS_CHANNEL_MASK_ID, &channelMask);
      else
        CS_WriteParameter(CS_CHANNEL_MASK_ID, &(ChannelsMask_t){1ul << currentChannel}); // limit channel mask to current channel only
      /* Make end device try to rejoin to any device on the same network it was before reset.
        Rejoin is performed on the channel which was used before reset first time, then on all channels
        from channel mask if rejoin on the initial channel fails. */
      setDeviceJoinParameters(true, true);
    }
  }
}

/**************************************************************************//**
\brief Starts network
******************************************************************************/
static void startNetwork(void)
{
  if (MAX_ATTEMPS_TO_ENTER_NETWORK == startNetworkAttempts++)
    return;

  zdoStartNetworkReq.ZDO_StartNetworkConf = ZDO_StartNetworkConf;
  ZDO_StartNetworkReq(&zdoStartNetworkReq);
}

/**************************************************************************//**
\brief Performs operations with network address which depends
  static or dinamyc on addressing type

\returns device type.
*******************************************************************************/
static DeviceType_t detectNwkAddrAndDevType(void)
{
  DeviceType_t deviceType;
#if (1 == CS_NWK_UNIQUE_ADDR)
  ShortAddr_t nwkAddr;

  // Read NWK address as dipswitch's state.
  nwkAddr = NWK_NODE_ADDRESS;
  if (0 == nwkAddr)
    deviceType = DEVICE_TYPE_COORDINATOR;
  else
    deviceType = DEVICE_TYPE_ROUTER;
  // Set parameter to config server
  CS_WriteParameter(CS_NWK_ADDR_ID, &nwkAddr);
#else // (1 != CS_NWK_UNIQUE_ADDR)
  deviceType = APP_DEVICE_TYPE;
#endif // (1 == CS_NWK_UNIQUE_ADDR)
  return deviceType;
}

/**************************************************************************//**
\brief Confirm on ZDO Reset Network

\param[in] conf - confiramtion parameters
******************************************************************************/
static void zdoResetNwkConf(ZDO_ResetNetworkConf_t *conf)
{
  (void)conf;
  identifyUpdateCommissioningState(false, false);
  startNetworkAttempts = 0;
  appState = APP_INITIAL_STATE;
  SYS_PostTask(APL_TASK_ID);
}

/**************************************************************************//**
\brief updates network join parameters for end devices & Routers

\param[in] discoverNetworks - NWK layer will performs active scan to find network,
  if it is true, NWK will try to join to neighbors from neighbor table if false;
\param[in] clearNeighborTable - The flag indicates that the neighbor table
  must be cleared or not before joining.
******************************************************************************/
static void setDeviceJoinParameters(bool discoverNetworks, bool clearNeighborTable)
{
  NWK_JoinControl_t joinControl;
  DeviceType_t deviceType;

  CS_ReadParameter(CS_DEVICE_TYPE_ID, &deviceType);

  if (DEVICE_TYPE_END_DEVICE == deviceType)
  {
    joinControl.annce = true;
    joinControl.discoverNetworks = discoverNetworks;
    joinControl.secured = true;
    joinControl.method = NWK_JOIN_VIA_REJOIN;
    joinControl.clearNeighborTable = clearNeighborTable;
    CS_WriteParameter(CS_JOIN_CONTROL_ID, &joinControl);
  }
  else if (DEVICE_TYPE_ROUTER == deviceType) /* arguments are not valid for router device */
  {
    joinControl.annce = false;
    joinControl.discoverNetworks = false;
    joinControl.secured = true;
    joinControl.method = NWK_JOIN_VIA_COMMISSIONING;
    joinControl.clearNeighborTable = false;
    CS_WriteParameter(CS_JOIN_CONTROL_ID, &joinControl);
  }
}

/**********************************************************************//**
\brief Main - C program main start function
**************************************************************************/
int main(void)
{

  halInitFirstLed();
  //halOnFirstLed();

  halInitSecondLed();
  halOffSecondLed();

  halInitThirdLed();
  halOffThirdLed();

  SYS_SysInit();

  for (;;)
  {
    SYS_RunTask();
#if defined ZAPPSI_HOST && defined WIN
    pollKeyboard();
#endif
  }
}

// eof zclDevice.c
