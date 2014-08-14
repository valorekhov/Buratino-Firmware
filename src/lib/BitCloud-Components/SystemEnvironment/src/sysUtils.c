/**********************************************************************//**
  \file sysUtils.c

  \brief

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2014, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    24/07/09 D. Kasyanov - Created
**************************************************************************/

/******************************************************************************
                                   Include section
******************************************************************************/
#include <sysUtils.h>
#include <rfRandom.h>
#include <appTimer.h>
#include <macEnvironment.h>

/******************************************************************************
                        Definitions section
******************************************************************************/
#define SYS_UPDATE_RAND_SEED_TIMEOUT  1000u

/******************************************************************************
                                   Prototypes section
******************************************************************************/
static void sysUpdateRndSeedTimerFired(void);
static void sysRndConfirm(RF_RandomConf_t *conf);
static uint16_t sysPseudoRandomSeed(void);

/******************************************************************************
                    Static variables section
******************************************************************************/
static HAL_AppTimer_t sysRndSeedTimer =
{
  .mode = TIMER_ONE_SHOT_MODE,
  .interval = SYS_UPDATE_RAND_SEED_TIMEOUT,
  .callback = sysUpdateRndSeedTimerFired
};

static RF_RandomReq_t sysRfRndReq =
{
  .disableRx = true
};

/******************************************************************************
                                   Implementation section
******************************************************************************/
/******************************************************************************
  Performs swap bytes in array of length
  Parameters:
    array  - pointer to array.
    length  - array length
  Returns:
    None
****************************************************************************/
void SYS_Swap(uint8_t *array, uint8_t length)
{
  uint8_t *dst = array + length - 1;

  uint8_t tmp;
  length >>= 1;

  while (length--)
  {
    tmp = *array;
    *array = *dst;
    *dst = tmp;

    array++;
    dst--;
  }
}

/*************************************************************************//**
\brief Updates rnd with new random values according to time interval
*****************************************************************************/
static void sysUpdateRndSeedTimerFired(void)
{
  RF_RandomReq(&sysRfRndReq);
}

/**************************************************************************//**
\brief Confirm from random number generation updater

\param[in]
    conf - confirm parameters
******************************************************************************/
static void sysRndConfirm(RF_RandomConf_t *conf)
{
  if (conf->value)
    srand(conf->value);
  else
    srand(sysPseudoRandomSeed());

  sysRfRndReq.disableRx = true;
  HAL_StartAppTimer(&sysRndSeedTimer);
}

/**************************************************************************//**
\brief  Starts timer to update rand
******************************************************************************/
void sysStartUpdatingRandSeed(void)
{
  if (false == sysRfRndReq.disableRx)
    return; // random request has been started already

  HAL_StopAppTimer(&sysRndSeedTimer);
  sysRfRndReq.RF_RandomConf = sysRndConfirm;
  sysRfRndReq.disableRx = false;
  sysUpdateRndSeedTimerFired();
}

/**************************************************************************//**
\brief Calculates a pseudo random value for seed if HW random is not available.

\return:
    16-bit value which was calculated by using current time and other params.
******************************************************************************/
static uint16_t sysPseudoRandomSeed(void)
{
  uint16_t result = (uint16_t)HAL_GetSystemTime();
  const uint16_t *extAddr = (const uint16_t *)MAC_GetExtAddr();
  uint8_t i;

  for (i = 0; i < sizeof(ExtAddr_t)/sizeof(result); ++i)
    result ^= extAddr[i];

  return result;
}

/**************************************************************************//**
  \brief This function copies size bytes of random data into buffer.

  \param: buffer - This is an unsigned char array of size at least sz to hold
   the random data.
  \param: size - The number of bytes of random data to compute and store.

  \return:  0 Indicates successful completion.
******************************************************************************/
int SYS_GetRandomSequence(uint8_t *buffer, unsigned long size)
{
  uint16_t i;
  u16Packed_t *u16ptr = (u16Packed_t*)buffer;

  for(i = 0; i < size/2; i++, u16ptr++)
    u16ptr->val = SYS_GetRandomNumber();

  if (size & (1<<0))
    buffer[size-1] = (uint8_t)SYS_GetRandomNumber();

  return 0;
}

#if defined(_SLEEP_WHEN_IDLE_)
/**************************************************************************//**
  \brief Stops priodic updating of the random seed.
 ******************************************************************************/
void sysStopUpdatingRandSeed(void)
{
  HAL_StopAppTimer(&sysRndSeedTimer);
}
#endif /* _SLEEP_WHEN_IDLE_ */

// eof sysUtils.c
