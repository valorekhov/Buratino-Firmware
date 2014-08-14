/*********************************************************************************************//**
\file

\brief

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal

$Id: N_Task.h 24615 2013-02-27 05:14:24Z arazinkov $

***************************************************************************************************/

#ifndef N_TASK_H
#define N_TASK_H

/***************************************************************************************************
* INCLUDE FILES
***************************************************************************************************/

#include "N_Types.h"

/***************************************************************************************************
* C++ DECLARATION WRAPPER
***************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************************************
* EXPORTED TYPES
***************************************************************************************************/

/** The task identifier.
    Components should call N_Task_GetTaskIdFromEventHandler() to get their task's N_Task_Id_t,
    which is typically done in the task's Init() function.
    0 is an invalid N_Task_Id_t, which is used to flag non-initialised task variables.
*/
typedef uint8_t N_Task_Id_t;

/** The event within the task (0..31).
*/
typedef uint8_t N_Task_Event_t;

/** Prototype of a task's event handler.
    \returns Whether the event was handled. N_Task will assert when TRUE is returned.
*/
typedef bool (*N_Task_HandleEvent_t)(N_Task_Event_t);

/***************************************************************************************************
* EXPORTED MACROS AND CONSTANTS
***************************************************************************************************/

/** This value of N_Task_Id_t is not used.
    The value zero is used so an unintentionally uninitialised static variable automatically uses it.
*/
#define N_TASK_ID_NONE 0x00u

/***************************************************************************************************
* EXPORTED FUNCTIONS
***************************************************************************************************/

/** Set an event.
    \param task The task ID (as returned by N_Task_GetTaskIdFromEventHandler()) of the task to set theevent to.
    \param evt The event to set.
*/
void N_Task_SetEvent(N_Task_Id_t task, N_Task_Event_t evt);

/** Clear an event.
    \param task The task ID (as returned by N_Task_GetTaskIdFromEventHandler()) of the task to clear the event from.
    \param evt The event to clear.
*/
void N_Task_ClearEvent(N_Task_Id_t task, N_Task_Event_t evt);

/** Get the task ID for a task, given its event handler.
    \param pfTaskEventHandler The event handler function of the task
    \returns The N_Task_Id_t of the task.
*/
N_Task_Id_t N_Task_GetIdFromEventHandler(const N_Task_HandleEvent_t pfTaskEventHandler);

/***************************************************************************************************
* END OF C++ DECLARATION WRAPPER
***************************************************************************************************/

#ifdef __cplusplus
}
#endif

#endif // N_TASK_H
