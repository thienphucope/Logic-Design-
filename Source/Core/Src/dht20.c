/*
 * dht20.c
 *
 *  Created on: Nov 8, 2024
 *      Author: HPVictus
 */

#include "DHT20.h"
#include "i2c.h"  // Bao gồm header của I2C để sử dụng giao tiếp I2C

// Định nghĩa các biến toàn cục
float RH = 0;
float Temp = 0;

// Hàm kiểm tra CRC
unsigned char DHT20_CheckCrc8(unsigned char *pDat, unsigned char Length)
{
    unsigned char crc = 0xff, i, j;

    for (i = 0; i < Length; i++)
    {
        crc = crc ^ *pDat;
        for (j = 0; j < 8; j++)
        {
            if (crc & 0x80) crc = (crc << 1) ^ 0x31;
            else crc <<= 1;
        }
        pDat++;
    }
    return crc;
}

// Đọc giá trị từ một thanh ghi của DHT20
uint8_t DHT20_ReadRegister(uint8_t register_pointer)
{
    HAL_StatusTypeDef status = HAL_OK;
    uint8_t return_value = 0;

    status = HAL_I2C_Mem_Read(&hi2c1, DHT20_I2C_ADDRESS << 1, (uint16_t)register_pointer, I2C_MEMADD_SIZE_8BIT, &return_value, 1, 100);

    if (status != HAL_OK)
    {
        // Nếu có lỗi, có thể thêm code xử lý lỗi
    }
    return return_value;
}

// Trigger đo lường và lấy dữ liệu từ DHT20
Status_Trigger_DHT20 DHT20_TriggerMeasurement(void)
{
    Status_Trigger_DHT20 status = DHT20_OK;

    HAL_Delay(100);
    uint8_t status_init = DHT20_ReadRegister(0x71);

    // Nếu cảm biến sẵn sàng để đo
    if ((status_init & 0x18) == 0x18)
    {
        HAL_Delay(10);
        uint8_t data_t[3] = {0xAC, 0x33, 0x00};
        HAL_I2C_Master_Transmit(&hi2c1, DHT20_I2C_ADDRESS << 1, (uint8_t *)data_t, 3, 100);
        HAL_Delay(80);

        uint8_t buffer[7];
        uint32_t data_read = 0;

        // Nhận dữ liệu
        HAL_I2C_Master_Receive(&hi2c1, DHT20_I2C_ADDRESS << 1, buffer, 7, 100);

        // Kiểm tra CRC và tính toán độ ẩm, nhiệt độ
        if ((buffer[0] & 0x80) == 0x00)
        {
            if (DHT20_CheckCrc8(&buffer[0], 6) == buffer[6])
            {
                data_read = buffer[1];
                data_read = data_read << 8;
                data_read += buffer[2];
                data_read = data_read << 8;
                data_read += buffer[3];
                data_read = data_read >> 4;

                RH = (float)data_read * 100 / 1048576;

                data_read = buffer[3] & 0x0F;
                data_read = data_read << 8;
                data_read += buffer[4];
                data_read = data_read << 8;
                data_read += buffer[5];

                Temp = (float)data_read * 200 / 1048576 - 50;
            }
            else
            {
                status = DHT20_ERROR_CRC;
            }
        }
        else
        {
            status = DHT20_BUSY;
        }
    }
    else
    {
        status = DHT20_ERROR_INIT;
    }
    return status;
}
