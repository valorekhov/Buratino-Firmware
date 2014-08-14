/**************************************************************************//**
  \file wlPdsSecurity.c

  \brief PDS security implementation.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    04.06.13 N. Fomin - Created.
******************************************************************************/

#if PDS_ENABLE_WEAR_LEVELING == 1
#ifdef _ENABLE_PERSISTENT_SERVER_
#ifdef PDS_SECURITY_CONTROL_ENABLE
/******************************************************************************
                    Includes section
******************************************************************************/
#include <wlPdsMemIds.h>
#include <csDefaults.h>
#include <nwkSecurity.h>
#include <wlPdsSecurity.h>
#include <wlPdsTypesConverter.h>
#include <eeprom.h>
#include <N_ErrH.h>
#include <sysUtils.h>
#include <csBuffers.h>

/******************************************************************************
                    Definitions section
******************************************************************************/
#define EEPROM_STORAGE_BASE_ADDRESS 8U
#define COMPID "wlPdsSecurity"
#define MAGIC_NUMBER 0x5AU

#define NWK_KEYS_STORAGE_AREA_ONE_START  EEPROM_STORAGE_BASE_ADDRESS
#define NWK_KEYS_STORAGE_AREA_TWO_START  (NWK_KEYS_STORAGE_AREA_ONE_START + \
  sizeof(PdsSecureItemHeader_t) + SECURITY_KEYS_ITEM_SIZE)
#define LINK_KEYS_STORAGE_AREA_ONE_START (NWK_KEYS_STORAGE_AREA_TWO_START + \
  sizeof(PdsSecureItemHeader_t) + SECURITY_KEYS_ITEM_SIZE)
#define LINK_KEYS_STORAGE_AREA_TWO_START (LINK_KEYS_STORAGE_AREA_ONE_START + \
  sizeof(PdsSecureItemHeader_t) + KEY_PAIR_DESCRIPTOR_ITEM_SIZE)

#define ITEM_ID_TO_ADDRESS_MAPPING(item, address1, address2) \
  {.itemId = item, .areaOneStartAddress = address1, .areaTwoStartAddress = address2}

/******************************************************************************
                    Types section
******************************************************************************/
BEGIN_PACK
typedef struct PACK
{
  uint8_t        magicNumber;
  S_Nv_ItemId_t id;
  uint16_t       size;
  uint16_t       crc;
  uint16_t       version;
} PdsSecureItemHeader_t;
END_PACK

typedef enum
{
  PDS_SECURITY_IDLE,
  PDS_SECURITY_WRITING_DATA,
  PDS_SECURITY_WRITING_HEADER
} PdsSecurityState_t;

typedef struct
{
  S_Nv_ItemId_t id;
  uint16_t       size;
  void           *data;
} PdsDataDescriptor_t;

typedef struct
{
  void     *data;
  uint16_t startAddress;
} PdsSecurityMem_t;

typedef struct
{
  S_Nv_ItemId_t itemId;
  uint16_t       areaOneStartAddress;
  uint16_t       areaTwoStartAddress;
} ItemIdToEepromOffsetMapping_t;

/******************************************************************************
                    Prototypes section
******************************************************************************/
static uint16_t pdsCalculateCrc(uint16_t size, uint8_t *data);
static void initializeEepromDescriptor(HAL_EepromParams_t *params, uint16_t address, uint8_t *data, uint16_t length);
static void writeDone(void);
static bool isDataModified(uint16_t eepromOffset, uint8_t *ramData, uint16_t size, uint16_t crc);
static bool isEmpty(uint8_t *data, uint16_t size);
static bool readData(PdsSecureItemHeader_t *header, uint16_t address, void *data, uint16_t size);
static int8_t getEepromItemPosition(S_Nv_ItemId_t id);

/******************************************************************************
                    Static variables section
******************************************************************************/
static PdsSecureItemHeader_t itemHeader;
static PdsSecurityState_t state;
static PdsSecurityMem_t pdsSecMem;
static PdsDataDescriptor_t postoponedStoring;
static ItemIdToEepromOffsetMapping_t PROGMEM_DECLARE(pdsSecMemoryMap[]) =
{
  ITEM_ID_TO_ADDRESS_MAPPING(CS_NWK_SECURITY_KEYS_ITEM_ID,
    NWK_KEYS_STORAGE_AREA_ONE_START,
    NWK_KEYS_STORAGE_AREA_TWO_START),
  ITEM_ID_TO_ADDRESS_MAPPING(CS_APS_KEY_PAIR_DESCRIPTORS_ITEM_ID,
    LINK_KEYS_STORAGE_AREA_ONE_START,
    LINK_KEYS_STORAGE_AREA_TWO_START)
};

/******************************************************************************
                    Implementation section
******************************************************************************/
/******************************************************************************
\brief Erases item under security control
******************************************************************************/
void PDS_EraseSecureItems(void)
{
  HAL_EepromParams_t eepromParams;
  ItemIdToEepromOffsetMapping_t mapItem;

  HAL_StopWriteToEeprom();

  for (uint16_t i = 0U; i < ARRAY_SIZE(pdsSecMemoryMap); i++)
  {
    memcpy_P(&mapItem, &pdsSecMemoryMap[i], sizeof(ItemIdToEepromOffsetMapping_t));

    initializeEepromDescriptor(&eepromParams, mapItem.areaOneStartAddress, (uint8_t *)&itemHeader, sizeof(PdsSecureItemHeader_t));
    N_ERRH_ASSERT_FATAL(0U == HAL_ReadEeprom(&eepromParams, NULL));
    itemHeader.crc += 1U;
    N_ERRH_ASSERT_FATAL(0U == HAL_WriteEeprom(&eepromParams, NULL));

    initializeEepromDescriptor(&eepromParams, mapItem.areaTwoStartAddress, (uint8_t *)&itemHeader, sizeof(PdsSecureItemHeader_t));
    N_ERRH_ASSERT_FATAL(0U == HAL_ReadEeprom(&eepromParams, NULL));
    itemHeader.crc += 1U;
    N_ERRH_ASSERT_FATAL(0U == HAL_WriteEeprom(&eepromParams, NULL));
  }
}

/******************************************************************************
\brief Stores item under security control

\param[in] id   - item id;
\param[in] size - item size;
\param[in] data - item data
******************************************************************************/
void pdsStoreSecuredItem(S_Nv_ItemId_t id, uint16_t size, void *data)
{
  PdsSecureItemHeader_t header1, header2;
  HAL_EepromParams_t eepromParams;
  int8_t pos = getEepromItemPosition(id);
  ItemIdToEepromOffsetMapping_t mapItem;

  N_ERRH_ASSERT_FATAL(-1 != pos);

  if (PDS_SECURITY_IDLE != state)
  {
    postoponedStoring.id   = id;
    postoponedStoring.size = size;
    postoponedStoring.data = data;
    return;
  }

  memcpy_P(&mapItem, &pdsSecMemoryMap[pos], sizeof(ItemIdToEepromOffsetMapping_t));
  // find "sector" with the latest data written
  initializeEepromDescriptor(&eepromParams, mapItem.areaOneStartAddress, (uint8_t *)&header1, sizeof(PdsSecureItemHeader_t));
  N_ERRH_ASSERT_FATAL(0U == HAL_ReadEeprom(&eepromParams, NULL));
  initializeEepromDescriptor(&eepromParams, mapItem.areaTwoStartAddress, (uint8_t *)&header2, sizeof(PdsSecureItemHeader_t));
  N_ERRH_ASSERT_FATAL(0U == HAL_ReadEeprom(&eepromParams, NULL));
  itemHeader.version = 1U;
  // check for empty data; the first write operation to eeprom
  if (isEmpty((uint8_t *)&header1, sizeof(PdsSecureItemHeader_t)))
  {
    // the first write operation to the first "sector"; the first write at all
    initializeEepromDescriptor(&eepromParams, mapItem.areaOneStartAddress, (uint8_t *)&itemHeader, sizeof(PdsSecureItemHeader_t));
    pdsSecMem.startAddress = mapItem.areaOneStartAddress;
  }
  else if (isEmpty((uint8_t *)&header2, sizeof(PdsSecureItemHeader_t)))
  {
    // the first write operation to the second "sector"
    if (!isDataModified(mapItem.areaOneStartAddress + sizeof(PdsSecureItemHeader_t), data, size, header1.crc))
      return;
    initializeEepromDescriptor(&eepromParams, mapItem.areaTwoStartAddress, (uint8_t *)&itemHeader, sizeof(PdsSecureItemHeader_t));
    pdsSecMem.startAddress = mapItem.areaTwoStartAddress;
  }
  else if ((header1.version > header2.version) || (UINT16_MAX == header2.version))
  {
    if (!isDataModified(mapItem.areaOneStartAddress + sizeof(PdsSecureItemHeader_t), data, size, header1.crc))
      return;
    itemHeader.version = header1.version + 1U;
    initializeEepromDescriptor(&eepromParams, mapItem.areaTwoStartAddress, (uint8_t *)&itemHeader, sizeof(PdsSecureItemHeader_t));
    pdsSecMem.startAddress = mapItem.areaTwoStartAddress;
  }
  else
  {
    if (!isDataModified(mapItem.areaTwoStartAddress + sizeof(PdsSecureItemHeader_t), data, size, header2.crc))
      return;
    itemHeader.version = header2.version + 1U;
    initializeEepromDescriptor(&eepromParams, mapItem.areaOneStartAddress, (uint8_t *)&itemHeader, sizeof(PdsSecureItemHeader_t));
    pdsSecMem.startAddress = mapItem.areaOneStartAddress;
  }
  pdsSecMem.startAddress += sizeof(PdsSecureItemHeader_t);

  state = PDS_SECURITY_WRITING_HEADER;

  pdsSecMem.data         = data;
  itemHeader.magicNumber = MAGIC_NUMBER;
  itemHeader.id          = id;
  itemHeader.size        = size;
  itemHeader.crc         = pdsCalculateCrc(size, data);
  N_ERRH_ASSERT_FATAL(0U == HAL_WriteEeprom(&eepromParams, writeDone));
}

/******************************************************************************
\brief Restores item under security control

\param[in] id   - item id;
\param[in] size - item size;
\param[in] data - item data
\returns true if item is restored successfully, false - otherwise
******************************************************************************/
bool pdsRestoreSecuredItem(S_Nv_ItemId_t id, uint16_t size, void *data)
{
  HAL_EepromParams_t eepromParams;
  PdsSecureItemHeader_t header1, header2;
  int8_t pos;
  ItemIdToEepromOffsetMapping_t mapItem;

  N_ERRH_ASSERT_FATAL(PDS_SECURITY_IDLE == state);
  pos = getEepromItemPosition(id);
  N_ERRH_ASSERT_FATAL(-1 != pos);

  memcpy_P(&mapItem, &pdsSecMemoryMap[pos], sizeof(ItemIdToEepromOffsetMapping_t));
  // find the latest version of item stored
  initializeEepromDescriptor(&eepromParams, mapItem.areaOneStartAddress, (uint8_t *)&header1, sizeof(PdsSecureItemHeader_t));
  N_ERRH_ASSERT_FATAL(0U == HAL_ReadEeprom(&eepromParams, NULL));
  initializeEepromDescriptor(&eepromParams, mapItem.areaTwoStartAddress, (uint8_t *)&header2, sizeof(PdsSecureItemHeader_t));
  N_ERRH_ASSERT_FATAL(0U == HAL_ReadEeprom(&eepromParams, NULL));

  if (header1.version > header2.version)
  {
    if (!readData(&header1, mapItem.areaOneStartAddress + sizeof(PdsSecureItemHeader_t), data, size))
      return readData(&header2, mapItem.areaTwoStartAddress + sizeof(PdsSecureItemHeader_t), data, size);
  }
  else
  {
    if (!readData(&header2, mapItem.areaTwoStartAddress + sizeof(PdsSecureItemHeader_t), data, size))
      return readData(&header1, mapItem.areaOneStartAddress + sizeof(PdsSecureItemHeader_t), data, size);
  }
  return true;
}

/******************************************************************************
\brief Calculates crc of item data

\param[in] size - item size;
\param[in] data - item data
\returns crc
******************************************************************************/
static uint16_t pdsCalculateCrc(uint16_t size, uint8_t *data)
{
  uint16_t crc = 0U;

  for (uint16_t i = 0U; i < size; i++)
    crc = SYS_Crc16Ccitt(crc, data[i]);

  return crc;
}

/******************************************************************************
\brief Initializes EEPROM descriptor

\param[in] params  - pointer to strucure with eeprom descriptor;
\param[in] address - address in EEPROM to write to;
\param[in] data    - item data;
\param[in] length  - item length
******************************************************************************/
static void initializeEepromDescriptor(HAL_EepromParams_t *params, uint16_t address, uint8_t *data, uint16_t length)
{
  params->address = address;
  params->data    = data;
  params->length  = length;
}

/******************************************************************************
\brief Writing to EEPROM is done
******************************************************************************/
static void writeDone(void)
{
  switch(state)
  {
    case PDS_SECURITY_WRITING_HEADER:
    {
      HAL_EepromParams_t eepromParams;

      if (pdsSecMem.data)
      {
        initializeEepromDescriptor(&eepromParams, pdsSecMem.startAddress, (uint8_t *)pdsSecMem.data, itemHeader.size);
        state = PDS_SECURITY_WRITING_DATA;
        HAL_WriteEeprom(&eepromParams, writeDone);
        break;
      }
    }
    case PDS_SECURITY_WRITING_DATA:
    {
      state = PDS_SECURITY_IDLE;
      if (0U != postoponedStoring.size)
      {
        pdsStoreSecuredItem(postoponedStoring.id, postoponedStoring.size, postoponedStoring.data);
        postoponedStoring.size = 0U;
      }
      break;
    }
    default:
      N_ERRH_ASSERT_FATAL(true);
  }
}

/******************************************************************************
\brief Checks whether data in EEPROM differs from data in RAM

\param[in] eepromOffset - ofsset of data in eeprom;
\param[in] ramData      - pointer to data in RAM;
\param[in] size         - data size;
\param[in] crc          - header crc
\returns true if data differs, false otherwise
******************************************************************************/
static bool isDataModified(uint16_t eepromOffset, uint8_t *ramData, uint16_t size, uint16_t crc)
{
  uint8_t readBuffer[16U];
  HAL_EepromParams_t eepromParams;
  uint8_t i = 0U;
  uint16_t ramCrc;

  ramCrc = pdsCalculateCrc(size, ramData);
  if (crc != ramCrc)
    return true;

  // compare data in eeprom with data in ram
  for (; i < size / 16U; i++)
  {
    initializeEepromDescriptor(&eepromParams, eepromOffset + i * 16U, readBuffer, 16U);
    if (0U != HAL_ReadEeprom(&eepromParams, NULL))
      return true;
    if (memcmp((uint8_t *)ramData + i * 16U, readBuffer, 16U))
      return true;
  }
  if (size % 16U)
  {
    initializeEepromDescriptor(&eepromParams, eepromOffset + i * 16U, readBuffer, size % 16U);
    if (0U != HAL_ReadEeprom(&eepromParams, NULL))
      return true;
    if (memcmp((uint8_t *)ramData + i * 16U, readBuffer, size % 16U))
      return true;
  }

  return false;
}

/******************************************************************************
\brief Checks whether data in EEPROM is empty

\param[in] data - item data;
\param[in] size - size of data
\returns true if data is empty, false otherwise
******************************************************************************/
static bool isEmpty(uint8_t *data, uint16_t size)
{
  for (uint8_t i = 0U; i < size; i++)
    if (0xFFU != data[i])
      return false;

  return true;
}

/******************************************************************************
\brief Reads data

\param[in] header  - pointer to item header structure;
\param[in] address - start address of read operation in eeprom;
\param[in] data    - item data;
\param[in] size    - size of data
\returns true if data is read correctly, false otherwise
******************************************************************************/
static bool readData(PdsSecureItemHeader_t *header, uint16_t address, void *data, uint16_t size)
{
  HAL_EepromParams_t eepromParams;
  uint16_t crc;

  if (MAGIC_NUMBER != header->magicNumber)
    return false;

  initializeEepromDescriptor(&eepromParams, address, data, size);
  N_ERRH_ASSERT_FATAL(0U == HAL_ReadEeprom(&eepromParams, NULL));

  crc = pdsCalculateCrc(size, data);
  if (crc != header->crc)
    return false;

  return true;
}

/******************************************************************************
\brief Looks for item id in idToOffset table

\param[in] id - item id
\returns position of item in table if found, -1 otherwise
******************************************************************************/
static int8_t getEepromItemPosition(S_Nv_ItemId_t id)
{
  ItemIdToEepromOffsetMapping_t mapItem;

  for (uint8_t  i = 0U; i < ARRAY_SIZE(pdsSecMemoryMap); i++)
  {
    memcpy_P(&mapItem, &pdsSecMemoryMap[i], sizeof(ItemIdToEepromOffsetMapping_t));
    if (mapItem.itemId == id)
      return i;
  }

  return -1;
}
#else
void PDS_EraseSecureItems(void)
{
// a Stub function if security items are stored in flash
}
#endif // #ifdef PDS_SECURITY_CONTROL_ENABLE
#endif // _ENABLE_PERSISTENT_SERVER_
#endif // PDS_ENABLE_WEAR_LEVELING == 1
// eof wlPdsSecurity.c
