/**************************************************************************//**
  \file   bspUid.c

  \brief  Implementation of UID interface.

  \author
      Atmel Corporation: http://www.atmel.com \n
      Support email: avr@atmel.com

    Copyright (c) 2008-2014, Atmel Corporation. All rights reserved.
    Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      26/12/14 Prashanth.Udumula - Created
 ******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/

#if BSP_SUPPORT != BOARD_FAKE
/******************************************************************************
                   Includes section
******************************************************************************/
#include <gpio.h>
#if defined(PLATFORM_STK600)
  #include <halTwi.h>
  #include <halW1.h>
#endif
#if BSP_SUPPORT == BOARD_ATMEGA256RFR2_XPRO
#include <atomic.h>
#endif
/******************************************************************************
                   Define(s) section
******************************************************************************/
// twi eeprom(at24c02b) address on i2c bus
#define AT24_DEVICE_ADDRESS       0xA0
#define MAC_ADDRESS_AREA_SIZE     8
#define MAC_ADDRESS_POSITION      2
#define BSP_UID_I2C_PRESCALER     0
#define ACTION_WAITING_TIME       50
// High Byte of the user signature area
#define UID_PAGE1                 0x01
#define IEEE_START_ADDRESS        2
#define IEEE_END_ADDRESS          8 + IEEE_START_ADDRESS

/******************************************************************************
                   Types section
******************************************************************************/
/** \brief uid type. */
typedef union
{
  uint64_t uid;
  uint8_t array[sizeof(uint64_t)];
} BspUid_t;

/******************************************************************************
                   Global variables section
******************************************************************************/
static BspUid_t bspUid = {.uid = 0ull};

/******************************************************************************
                   Implementations section
******************************************************************************/
#if (BSP_SUPPORT == BOARD_RCB || BSP_SUPPORT == BOARD_RCB_KEY_REMOTE)
/**************************************************************************//**
  \brief Writes/reads byte to/from SPI.
  \param[in] value - byte to write.
  \return  the byte which was read.
******************************************************************************/
static uint8_t bspWriteByteSpi(uint8_t value)
{
  SPDR = value;
  asm("nop");   // This "nop" tunes up the "while" to reduce time for SPIF flag detecting.
  while (!(SPSR & (1 << SPIF)));
  return SPDR;
}

/**************************************************************************//**
Reads uid from external eeprom
******************************************************************************/
void bspReadUid(void)
{
  uint8_t command = 0x03;
  uint8_t address = 0;
  uint8_t itr;

  GPIO_SPI_CS_set();
  GPIO_SPI_MISO_make_in();
  GPIO_SPI_MOSI_make_out();
  GPIO_SPI_SCK_make_out();
  GPIO_HW_SPI_CS_make_out();
  GPIO_SPI_CS_make_out();

  SPCR = ((1 << SPE) | (1 << MSTR));               // SPI enable, master mode.

  GPIO_SPI_CS_clr();

  bspWriteByteSpi(command);
  bspWriteByteSpi(address);
  for (itr = 0; itr < sizeof(uint64_t); itr++)
    bspUid.array[itr] = bspWriteByteSpi(address);

  GPIO_SPI_CS_set();
  // disable spi
  SPCR = 0;

  GPIO_HW_SPI_CS_make_in();
  GPIO_SPI_MOSI_make_in();
  GPIO_SPI_SCK_make_in();
}
#elif defined(PLATFORM_STK600)
/**************************************************************************//**
 Reads uid from external eeprom
******************************************************************************/
void bspReadUid(void)
{
  uint8_t state;
  uint8_t itr;
  uint8_t waiting;

  TWCR = 0x00;
  TWSR = BSP_UID_I2C_PRESCALER; // prescaler
  // Set 250 Kb/s clock rate
  TWBR = (uint8_t)((F_CPU / 250000ul) - 16ul)/(2ul * (1ul << BSP_UID_I2C_PRESCALER) * (1ul << BSP_UID_I2C_PRESCALER));

  // disable twi interrupt
  TWCR &= (~(1 << TWIE));
  /////////////// start of dummy write  ////////////////////
  // send start condition
  TWCR = (1 << TWSTA) | (1 << TWINT) | (1 << TWEN);

  // wait for end of action
  waiting = ACTION_WAITING_TIME;
  while(!(TWCR & (1 << TWINT)) && waiting--)
    __delay_us(1);

  state = TWSR & 0xF8;
  if ((TWS_START != state) && (TWS_RSTART != state))
    return;

  // send at24 address + W
  TWDR =  AT24_DEVICE_ADDRESS;
  TWCR = (1 << TWINT) | (1 << TWEN);

  // wait for end of action
  waiting = ACTION_WAITING_TIME;
  while(!(TWCR & (1 << TWINT)) && waiting--)
    __delay_us(1);

  state = TWSR & 0xF8;
  if (TWS_MT_SLA_ACK != state)
    return;

  // send internal eeprom cell address
  for (itr = 0; itr < sizeof(uint16_t); ++itr)
  {
    // send address byte
    TWDR =  (uint8_t)((uint16_t)MAC_ADDRESS_POSITION >> (8 * itr));
    TWCR = (1 << TWINT) | (1 << TWEN);

    // wait for end of action
    while(!(TWCR & (1 << TWINT)));

    state = TWSR & 0xF8;
    if (TWS_MT_DATA_ACK != state)
      return;
  }
  /////////////// end of dummy write  ////////////////////
  /////////////// start of data read ////////////////////
  // send start condition
  TWCR = (1 << TWSTA) | (1 << TWINT) | (1 << TWEN);

  // wait for end of action
  while(!(TWCR & (1 << TWINT)));

  state = TWSR & 0xF8;
  if ((TWS_START != state) && (TWS_RSTART != state))
    return;

  // send at24 address + R
  TWDR =  AT24_DEVICE_ADDRESS | 0x01;
  TWCR = (1 << TWINT) | (1 << TWEN);

  // wait for end of action
  while(!(TWCR & (1 << TWINT)));

  state = TWSR & 0xF8;
  if (TWS_MR_SLA_ACK != state)
    return;

  // read MAC address
  for (itr = 0; itr < MAC_ADDRESS_AREA_SIZE; itr++)
  {
    if ((MAC_ADDRESS_AREA_SIZE - 1) == itr)
    { // send nack on last data byte
      TWCR &= ~(1 << TWEA);
      TWCR = (1 << TWINT) | (1 << TWEN);
    }
    else
    { // send ack
      TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
    }
    // wait for end of action
    while(!(TWCR & (1 << TWINT)));

    state = TWSR & 0xF8;
    if ((TWS_MR_DATA_ACK != state) && (TWS_MR_DATA_NACK != state))
      return;

    bspUid.array[MAC_ADDRESS_AREA_SIZE - itr - 1] = TWDR;
  }
  /////////////// end of data read ////////////////////
  // send stop condition
  TWCR = (1 << TWSTO) | (1 << TWINT) | (1 << TWEN);
  // wait for end of stop condition
  while(!(TWCR & (1 << TWSTO)));
}

#elif BSP_SUPPORT == BOARD_ATMEGA256RFR2_XPRO
void bspReadUid(void)
{  
  ATOMIC_SECTION_ENTER
  /* has an addr of 0x1E, used for storing IEEE addr read from flash */
  GPIOR0 = 0x00;   
  /* has an addr of 0x2A, used for storing addr of user page 1 */
  GPIOR1 = UID_PAGE1;
  /* has an addr of 0x2B, used to store the addr of flash data to be read */
  GPIOR2 = 0x00;      
  /* chooses the user signature page mode */
  NEMCR = 0x50;
  for (uint8_t index = IEEE_START_ADDRESS; index< IEEE_END_ADDRESS ; index++)
  {
    GPIOR2 = index;
    {
    ASM (
        "push r30                \n\t" /* pushing to stack as it is used */
        "push r31                \n\t" /* pushing to stack as it is used */
        "push r19                \n\t" /* pushing to stack as it is used */
        "in   r30,0x2B           \n\t" /* loading the GPIOR2 to Z pointer */
        "in   r31,0x2A           \n\t" /* loading the GPIOR1 to Z pointer */
        "ldi  r19,0x21           \n\t" /* Load (1<<SIGRD) | (1<<SPMEN) to R19 */
        "out  0x37,r19           \n\t" /* Write to SPMCSR */
        "lpm  r19,z              \n\t" /* Read from UserPage */
        "out  0x1E,r19           \n\t" /* storing the read data to GPIOR0 */
        "pop  r19                \n\t" /* poping from stack after using */
        "pop  r31                \n\t" /* poping from stack after using */
        "pop  r30                \n\t" /* poping from stack after using */
        );
    bspUid.array[index - IEEE_START_ADDRESS] = GPIOR0;
    }
  }
  NEMCR = 0x40;
  ATOMIC_SECTION_LEAVE
}
#endif

/******************************************************************************
 Returns number which was read from external eeprom.
 Parameters:
   id - UID buffer pointer.
 Returns:
   0 - if unique ID has been found without error;
  -1 - if there are some erros during UID discovery.
******************************************************************************/
int BSP_ReadUid(uint64_t *id)
{
  if (!id)
    return -1;
  
  bspReadUid();
  
  *id = bspUid.uid;
  return 0;
}
#endif // if BSP_SUPPORT != BOARD_FAKE
// eof bspUid.c
