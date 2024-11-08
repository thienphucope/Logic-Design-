/*
 * dht20.h
 *
 *  Created on: Nov 8, 2024
 *      Author: HPVictus
 */

#ifndef INC_DHT20_H_
#define INC_DHT20_H_

#include "stm32f1xx_hal.h"  // Hoặc include đúng header cho MCU bạn đang dùng

#define DHT20_I2C_ADDRESS 0x38  // Địa chỉ I2C của DHT20

// Khai báo các hàm và cấu trúc liên quan đến DHT20
typedef enum
{
  DHT20_OK            = 0x00U,
  DHT20_ERROR_INIT    = 0x01U,
  DHT20_ERROR_CRC     = 0x02U,
  DHT20_BUSY          = 0x03U
} Status_Trigger_DHT20;

extern float RH;        // Độ ẩm
extern float Temp;      // Nhiệt độ

// Các hàm khởi tạo và xử lý DHT20
HAL_StatusTypeDef DHT20_Init(I2C_HandleTypeDef *hi2c);
Status_Trigger_DHT20 DHT20_TriggerMeasurement(void);
uint8_t DHT20_ReadRegister(uint8_t register_pointer);
unsigned char DHT20_CheckCrc8(unsigned char *pDat, unsigned char Length);


#endif /* INC_DHT20_H_ */
