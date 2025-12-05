/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : usbd_cdc_if.c
  * @version        : v1.0_Cube
  * @brief          : Usb device for Virtual Com Port.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "usbd_cdc_if.h"

/* USER CODE BEGIN INCLUDE */

/* USER CODE END INCLUDE */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @brief Usb device library.
  * @{
  */

/** @addtogroup USBD_CDC_IF
  * @{
  */

/** @defgroup USBD_CDC_IF_Private_TypesDefinitions USBD_CDC_IF_Private_TypesDefinitions
  * @brief Private types.
  * @{
  */

/* USER CODE BEGIN PRIVATE_TYPES */

/* USER CODE END PRIVATE_TYPES */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Private_Defines USBD_CDC_IF_Private_Defines
  * @brief Private defines.
  * @{
  */

/* USER CODE BEGIN PRIVATE_DEFINES */
/* USER CODE END PRIVATE_DEFINES */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Private_Macros USBD_CDC_IF_Private_Macros
  * @brief Private macros.
  * @{
  */

/* USER CODE BEGIN PRIVATE_MACRO */

/* USER CODE END PRIVATE_MACRO */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Private_Variables USBD_CDC_IF_Private_Variables
  * @brief Private variables.
  * @{
  */
/* Create buffer for reception and transmission           */
/* It's up to user to redefine and/or remove those define */
/** Received data over USB are stored in this buffer      */
uint8_t UserRxBufferFS[APP_RX_DATA_SIZE];

/** Data to send over USB CDC are stored in this buffer   */
uint8_t UserTxBufferFS[APP_TX_DATA_SIZE];

/* USER CODE BEGIN PRIVATE_VARIABLES */
extern uint8_t CDC_Transmit_FS(uint8_t* Buf, uint16_t Len);
extern volatile uint8_t up_limit_reached;
extern volatile uint8_t down_limit_reached;
extern volatile uint8_t up_limit_reported;
extern volatile uint8_t down_limit_reported;
extern int direction;
extern int stop;
extern uint32_t speed;
extern int fw_bw;
extern int right;
extern int left;
extern uint8_t light;
extern uint8_t fw_mot_start;
/* USER CODE END PRIVATE_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Exported_Variables USBD_CDC_IF_Exported_Variables
  * @brief Public variables.
  * @{
  */

extern USBD_HandleTypeDef hUsbDeviceFS;

/* USER CODE BEGIN EXPORTED_VARIABLES */

/* USER CODE END EXPORTED_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Private_FunctionPrototypes USBD_CDC_IF_Private_FunctionPrototypes
  * @brief Private functions declaration.
  * @{
  */

static int8_t CDC_Init_FS(void);
static int8_t CDC_DeInit_FS(void);
static int8_t CDC_Control_FS(uint8_t cmd, uint8_t* pbuf, uint16_t length);
static int8_t CDC_Receive_FS(uint8_t* pbuf, uint32_t *Len);
static int8_t CDC_TransmitCplt_FS(uint8_t *pbuf, uint32_t *Len, uint8_t epnum);

/* USER CODE BEGIN PRIVATE_FUNCTIONS_DECLARATION */

/* USER CODE END PRIVATE_FUNCTIONS_DECLARATION */

/**
  * @}
  */

USBD_CDC_ItfTypeDef USBD_Interface_fops_FS =
{
  CDC_Init_FS,
  CDC_DeInit_FS,
  CDC_Control_FS,
  CDC_Receive_FS,
  CDC_TransmitCplt_FS
};

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Initializes the CDC media low layer over the FS USB IP
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Init_FS(void)
{
  /* USER CODE BEGIN 3 */
  /* Set Application Buffers */
  USBD_CDC_SetTxBuffer(&hUsbDeviceFS, UserTxBufferFS, 0);
  USBD_CDC_SetRxBuffer(&hUsbDeviceFS, UserRxBufferFS);
  return (USBD_OK);
  /* USER CODE END 3 */
}

/**
  * @brief  DeInitializes the CDC media low layer
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_DeInit_FS(void)
{
  /* USER CODE BEGIN 4 */
  return (USBD_OK);
  /* USER CODE END 4 */
}

/**
  * @brief  Manage the CDC class requests
  * @param  cmd: Command code
  * @param  pbuf: Buffer containing command data (request parameters)
  * @param  length: Number of data to be sent (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Control_FS(uint8_t cmd, uint8_t* pbuf, uint16_t length)
{
  /* USER CODE BEGIN 5 */
  switch(cmd)
  {
    case CDC_SEND_ENCAPSULATED_COMMAND:

    break;

    case CDC_GET_ENCAPSULATED_RESPONSE:

    break;

    case CDC_SET_COMM_FEATURE:

    break;

    case CDC_GET_COMM_FEATURE:

    break;

    case CDC_CLEAR_COMM_FEATURE:

    break;

  /*******************************************************************************/
  /* Line Coding Structure                                                       */
  /*-----------------------------------------------------------------------------*/
  /* Offset | Field       | Size | Value  | Description                          */
  /* 0      | dwDTERate   |   4  | Number |Data terminal rate, in bits per second*/
  /* 4      | bCharFormat |   1  | Number | Stop bits                            */
  /*                                        0 - 1 Stop bit                       */
  /*                                        1 - 1.5 Stop bits                    */
  /*                                        2 - 2 Stop bits                      */
  /* 5      | bParityType |  1   | Number | Parity                               */
  /*                                        0 - None                             */
  /*                                        1 - Odd                              */
  /*                                        2 - Even                             */
  /*                                        3 - Mark                             */
  /*                                        4 - Space                            */
  /* 6      | bDataBits  |   1   | Number Data bits (5, 6, 7, 8 or 16).          */
  /*******************************************************************************/
    case CDC_SET_LINE_CODING:

    break;

    case CDC_GET_LINE_CODING:

    break;

    case CDC_SET_CONTROL_LINE_STATE:

    break;

    case CDC_SEND_BREAK:

    break;

  default:
    break;
  }

  return (USBD_OK);
  /* USER CODE END 5 */
}

/**
  * @brief  Data received over USB OUT endpoint are sent over CDC interface
  *         through this function.
  *
  *         @note
  *         This function will issue a NAK packet on any OUT packet received on
  *         USB endpoint until exiting this function. If you exit this function
  *         before transfer is complete on CDC interface (ie. using DMA controller)
  *         it will result in receiving more data while previous ones are still
  *         not sent.
  *
  * @param  Buf: Buffer of data to be received
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Receive_FS(uint8_t* Buf, uint32_t *Len)
{
  /* USER CODE BEGIN 6 */
	 Buf[*Len] = '\0'; // Null-terminate

	    // Trim CR/LF
	    for (int i = 0; i < *Len; i++) {
	        if (Buf[i] == '\r' || Buf[i] == '\n') { Buf[i] = '\0'; break; }
	    }

	    // Lowercase the buffer
	    char cmd[32];
	    strncpy(cmd, (char*)Buf, sizeof(cmd)-1);
	    cmd[sizeof(cmd)-1] = '\0';
	    for (int i = 0; cmd[i]; i++) {
	        if (cmd[i] >= 'A' && cmd[i] <= 'Z') cmd[i] += 32;
	    }

	    if (strncmp((char*)Buf, "speed:", 6) == 0) {
	            speed = atoi((char*)(Buf + 6));

	        }

	    if (strcmp(cmd, "forward") == 0) {
	        //direction = 1;
	    	fw_bw = 1;
	        down_limit_reached = 0;
	        //down_limit_reported = 0;
	        CDC_Transmit_FS((uint8_t*)"ACK:forward\n", strlen("ACK:forward\n"));
	    }
	    else if (strcmp(cmd, "back") == 0 || strcmp(cmd, "backward") == 0) {
	        //direction = 0;
	    	fw_bw = 0;
	        up_limit_reached = 0;
	        //up_limit_reported = 0;
	        CDC_Transmit_FS((uint8_t*)"ACK:back\n", strlen("ACK:back\n"));
	    }
	    else if (strcmp(cmd, "left") == 0) {
	        CDC_Transmit_FS((uint8_t*)"ACK:left\n", strlen("ACK:left\n"));
	        left = 1;
	        right = 0;
	    }
	    else if (strcmp(cmd, "right") == 0) {
	        CDC_Transmit_FS((uint8_t*)"ACK:right\n", strlen("ACK:right\n"));
	        right = 1;
	        left = 0;
	    }
	    else if (strcmp(cmd, "dirstop") == 0) {
	    	        CDC_Transmit_FS((uint8_t*)"ACK:dirstop\n", strlen("ACK:dirstop\n"));
	    	        right = 0;
	    	        left = 0;
	    	    }
	    else if (strcmp(cmd, "up") == 0) {
	        if (up_limit_reached) {
	            CDC_Transmit_FS((uint8_t*)"LIMIT:UP\n", strlen("LIMIT:UP\n"));
	        } else {
	        	stop = 0;
	            direction = 1;
	            down_limit_reached = 0;
	            //down_limit_reported = 1;
	            CDC_Transmit_FS((uint8_t*)"ACK:up dir=1\n", strlen("ACK:up dir=1\n"));
	        }
	    }
	    else if (strcmp(cmd, "down") == 0) {
	        if (down_limit_reached) {
	            CDC_Transmit_FS((uint8_t*)"LIMIT:DOWN\n", strlen("LIMIT:DOWN\n"));
	        } else {
	        	stop = 0;
	            direction = 0;
	            up_limit_reached = 0;
	            //up_limit_reported = 1;
	            CDC_Transmit_FS((uint8_t*)"ACK:down dir=0\n", strlen("ACK:down dir=0\n"));
	        }
	    }
	    else if (strcmp(cmd, "stop") == 0) {
	    	stop = 1;
	        CDC_Transmit_FS((uint8_t*)"ACK:stop\n", strlen("ACK:stop\n"));
	    }
	    else if (strcmp(cmd, "start") == 0) {
	    	    	stop = 0;
	    	    	fw_mot_start = 1;
	    	    	CDC_Transmit_FS((uint8_t*)"ACK:start\n", strlen("ACK:start\n"));
	    	    }
	    else if (strcmp(cmd, "lon") == 0) {
	    	    	    	light = 1;
	    	    	    	CDC_Transmit_FS((uint8_t*)"ACK:lights_on\n", strlen("ACK:lights_on\n"));
	    	    	    }
	    else if (strcmp(cmd, "loff") == 0) {
	   	    	    	    	light = 0;
	   	    	    	    	CDC_Transmit_FS((uint8_t*)"ACK:lights_off\n", strlen("ACK:lights_off\n"));
	   	    	    	    }
	    else {
	        CDC_Transmit_FS((uint8_t*)"ACK:unknown\n", strlen("ACK:unknown\n"));
	    }

	    USBD_CDC_SetRxBuffer(&hUsbDeviceFS, &Buf[0]);
	    USBD_CDC_ReceivePacket(&hUsbDeviceFS);
	    return USBD_OK;
  /* USER CODE END 6 */
}

/**
  * @brief  CDC_Transmit_FS
  *         Data to send over USB IN endpoint are sent over CDC interface
  *         through this function.
  *         @note
  *
  *
  * @param  Buf: Buffer of data to be sent
  * @param  Len: Number of data to be sent (in bytes)
  * @retval USBD_OK if all operations are OK else USBD_FAIL or USBD_BUSY
  */
uint8_t CDC_Transmit_FS(uint8_t* Buf, uint16_t Len)
{
  uint8_t result = USBD_OK;
  /* USER CODE BEGIN 7 */
  USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef*)hUsbDeviceFS.pClassData;
  if (hcdc->TxState != 0){
    return USBD_BUSY;
  }
  USBD_CDC_SetTxBuffer(&hUsbDeviceFS, Buf, Len);
  result = USBD_CDC_TransmitPacket(&hUsbDeviceFS);
  /* USER CODE END 7 */
  return result;
}

/**
  * @brief  CDC_TransmitCplt_FS
  *         Data transmitted callback
  *
  *         @note
  *         This function is IN transfer complete callback used to inform user that
  *         the submitted Data is successfully sent over USB.
  *
  * @param  Buf: Buffer of data to be received
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_TransmitCplt_FS(uint8_t *Buf, uint32_t *Len, uint8_t epnum)
{
  uint8_t result = USBD_OK;
  /* USER CODE BEGIN 13 */
  UNUSED(Buf);
  UNUSED(Len);
  UNUSED(epnum);
  /* USER CODE END 13 */
  return result;
}

/* USER CODE BEGIN PRIVATE_FUNCTIONS_IMPLEMENTATION */

/* USER CODE END PRIVATE_FUNCTIONS_IMPLEMENTATION */

/**
  * @}
  */

/**
  * @}
  */
