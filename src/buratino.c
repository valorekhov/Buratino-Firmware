

/******************************************************************************
                             Includes section
******************************************************************************/
#include <osOccupancySensingCluster.h>
#include "features/temperatureSensor/include/tmpTemperatureMeasurementCluster.h"
#include <osClusters.h>
#include <zclDevice.h>
#include <uartManager.h>
#include <zclSecurityManager.h>
#include <console.h>
#include <identifyCluster.h>
#include <sysIdleHandler.h>
#include <ezModeManager.h>
#include <pdsDataServer.h>
#include <haClusters.h>
#include <otauService.h>
#include <bspLeds.h>
#include <buratinoSettings.h>

/******************************************************************************
                             Defines section
******************************************************************************/
#define SWITCHING_PERIOD                 20000UL

/******************************************************************************
                        Prototypes section
******************************************************************************/
static void isBusyOrPollCheck(SYS_EventId_t eventId, SYS_EventData_t data);

/******************************************************************************
                    Local variables section
******************************************************************************/
static ZCL_DeviceEndpoint_t osEndpoint =
{
  .simpleDescriptor =
  {
    .endpoint            = APP_SRC_ENDPOINT_ID,
    .AppProfileId        = PROFILE_ID_HOME_AUTOMATION,
    .AppDeviceId         = HA_OCCUPANCY_SENSOR_DEVICE_ID,
    .AppInClustersCount  = ARRAY_SIZE(osServerClusterIds),
    .AppInClustersList   = osServerClusterIds,
    .AppOutClustersCount = ARRAY_SIZE(osClientClusterIds),
    .AppOutClustersList  = osClientClusterIds,
  },
  .serverCluster = osServerClusters,
  .clientCluster = osClientClusters,
};

static ClusterId_t osServerClusterToBindIds[] =
{
  OCCUPANCY_SENSING_CLUSTER_ID
};
static AppBindReq_t osBindReq =
{
  .nwkAddrOfInterest = RX_ON_WHEN_IDLE_ADDR,
  .remoteServers     = NULL,
  .remoteServersCnt  = 0,
  .remoteClients     = osServerClusterToBindIds,
  .remoteClientsCnt  = ARRAY_SIZE(osServerClusterToBindIds),
  .profile           = PROFILE_ID_HOME_AUTOMATION,
  .srcEndpoint       = APP_SRC_ENDPOINT_ID,
  .callback          = NULL,
};

static ZCL_LinkKeyDesc_t keyDesc = {CCPU_TO_LE64(APS_UNIVERSAL_EXTENDED_ADDRESS), HA_LINK_KEY};

static HAL_AppTimer_t occupancyChangeTimer =
{
  .interval = SWITCHING_PERIOD,
  .mode     = TIMER_REPEAT_MODE,
  .callback = occupancySensingToggleOccupancy,
};

static HAL_AppTimer_t tempMeasurementTimer =
{
	.interval = SWITCHING_PERIOD,
	.mode     = TIMER_REPEAT_MODE,
	.callback = tempRhMeasurementToggleTemperatureAndHumidity,
};

static SYS_EventReceiver_t zdoBusyPollCheck = { .func = isBusyOrPollCheck};

/******************************************************************************
                    Implementation section
******************************************************************************/
/**************************************************************************//**
\brief Device initialization routine
******************************************************************************/
void appDeviceInit(void)
{
  appDeviceInitAfterPowerFailure();
}

/**************************************************************************//**
\brief PCT initialization after power failure
******************************************************************************/
void appDeviceInitAfterPowerFailure(void)
{
  ZCL_RegisterEndpoint(&osEndpoint);


#if (APP_ENABLE_CONSOLE == 1) || (APP_DEVICE_EVENTS_LOGGING == 1)
  uartInit();
#endif
#if APP_ENABLE_CONSOLE == 1
  initConsole();
#endif

#ifdef BURATINO_CAPABILITY_TEMPRH_SENSOR
  HAL_StartAppTimer(&occupancyChangeTimer);
  occupancySensingClusterInit();
#endif
#ifdef BURATINO_CAPABILITY_TEMPRH_SENSOR
	HAL_StartAppTimer(&tempMeasurementTimer);
	temperatureRhMeasurementClustersInit();
#endif  
  
#if defined (_SLEEP_WHEN_IDLE_) && (APP_ENABLE_CONSOLE != 1)
  SYS_EnableSleepWhenIdle();
#endif
  SYS_SubscribeToEvent(BC_EVENT_POLL_REQUEST, &zdoBusyPollCheck);

  if (PDS_IsAbleToRestore(APP_OS_OCCUPANCY_MEM_ID))
    PDS_Restore(APP_OS_OCCUPANCY_MEM_ID);

  // appEzModeDone();
  ZCL_StartReporting();
}

/**************************************************************************//**
\breif Performs security initialization actions
******************************************************************************/
void appSecurityInit(void)
{
  ZCL_Set_t zclSet;

  ZCL_ResetSecurity();

  zclSet.attr.id = ZCL_LINK_KEY_DESC_ID;
  zclSet.attr.value.linkKeyDesc = &keyDesc;
  ZCL_Set(&zclSet);  
}

/**************************************************************************//**
\brief Device common task handler
******************************************************************************/
void appDeviceTaskHandler(void)
{
  switch (appDeviceState) // Actual device state when one joined network
  {
    case DEVICE_INITIAL_STATE:
      {
        appDeviceState = DEVICE_ACTIVE_IDLE_STATE;
        startOtauClient(&osClientClusters[BURATINO_CLIENT_CLUSTERS_COUNT - 1]);
      }
      break;
    case DEVICE_ACTIVE_IDLE_STATE:
    default:
      break;
  }
}

/**************************************************************************//**
\brief Gets bind request

\return pointer to a bind request used by HA device
******************************************************************************/
AppBindReq_t *getDeviceBindRequest(void)
{
  return &osBindReq;
}

/**************************************************************************//**
\brief Stops application
******************************************************************************/
void appStop(void)
{
  HAL_StopAppTimer(&occupancyChangeTimer);
  identifyClusterStop();
}

/**************************************************************************//**
\brief Asks device if it is an initiator

\returns true if it is, false otherwise
******************************************************************************/
bool appIsInitiator(void)
{
  return false;
}

/**************************************************************************//**
\brief EZ-Mode done callback

\returns function which is called by EZ-Mode manager when it is done
******************************************************************************/
void appEzModeDone(void)
{
  sendConfigureReportingToNotify(APP_ENDPOINT_COMBINED_INTERFACE, OCCUPANCY_SENSING_CLUSTER_ID,
	ZCL_OCCUPANCY_SENSING_CLUSTER_OCCUPANCY_SERVER_ATTRIBUTE_ID, OCCUPANCY_SENSING_VAL_MAX_REPORT_PERIOD * 2);
  ZCL_StartReporting();
}

/**************************************************************************//**
\brief ZDO Binding indication function

\param[out] bindInd - ZDO bind indication parameters structure pointer.
                      For details go to ZDO_BindInd_t declaration
******************************************************************************/
void ZDO_BindIndication(ZDO_BindInd_t *bindInd)
{
  (void)bindInd;
  halOnThirdLed();	
}

/**************************************************************************//**
\brief ZDO Unbinding indication function

\param[out] unbindInd - ZDO unbind indication parameters structure pointer.
                        For details go to ZDO_UnbindInd_t declaration
******************************************************************************/
void ZDO_UnbindIndication(ZDO_UnbindInd_t *unbindInd)
{
  (void)unbindInd;
}

/**************************************************************************//**
  \brief Processes BC_EVENT_POLL_REQUEST event

  \param[in] eventId - id of raised event;
  \param[in] data    - event's data.
******************************************************************************/
static void isBusyOrPollCheck(SYS_EventId_t eventId, SYS_EventData_t data)
{
#if defined (_SLEEP_WHEN_IDLE_) && (APP_ENABLE_CONSOLE != 1)
  bool *check = (bool *)data;

  *check |= isEzModeInProgress();
#else
  (void)eventId, (void)data;
#endif
}

// eof buratino.c