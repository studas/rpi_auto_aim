#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "esp_log.h"
#include "driver/i2c.h"

#include "i2c_slave.hpp"

static const char *TAG = "i2c_slave";

#define I2C_SLAVE_TX_BUF_LEN 255                        /*!< I2C master doesn't need buffer */
#define I2C_SLAVE_RX_BUF_LEN 255                           /*!< I2C master doesn't need buffer */

#define WRITE_BIT I2C_MASTER_WRITE              /*!< I2C master write */
#define READ_BIT I2C_MASTER_READ                /*!< I2C master read */
#define ACK_CHECK_EN 0x1                        /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS 0x0                       /*!< I2C master will not check ack from slave */
#define ACK_VAL 0x0                             /*!< I2C ack value */
#define NACK_VAL 0x1                            /*!< I2C nack value */

I2CSlave::I2CSlave(uint16_t sda_io, uint16_t scl_io, uint16_t addr, int port) 
    : sda_io(sda_io), scl_io(scl_io), addr(addr) {
    this->port = port ? I2C_NUM_1 : I2C_NUM_0; 

    i2c_config_t conf_slave = {
        .mode = I2C_MODE_SLAVE,
        .sda_io_num = this->sda_io,          // select GPIO specific to your project
        .scl_io_num = this->scl_io,          // select GPIO specific to your project
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .slave{
            .addr_10bit_en = 0,
            .slave_addr = this->addr,      // address of your project
        },
        .clk_flags = 0,
    };
    ESP_ERROR_CHECK(i2c_param_config(this->port, &conf_slave));
    ESP_ERROR_CHECK(i2c_driver_install(this->port, conf_slave.mode, I2C_SLAVE_RX_BUF_LEN, I2C_SLAVE_TX_BUF_LEN, 0));
    ESP_LOGI(TAG, "I2C Slave initialized successfully");
}

I2CSlave::~I2CSlave() {
    i2c_driver_delete(this->port);
}

char *I2CSlave::read_buffer() {
    uint8_t *received_data = (uint8_t*)malloc(I2C_SLAVE_RX_BUF_LEN);
    if (received_data == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for received data");
        return NULL;
    }
    memset(received_data, 0, I2C_SLAVE_RX_BUF_LEN);
    received_data[0] = '\0';
    
    i2c_slave_read_buffer(this->port, received_data, I2C_SLAVE_RX_BUF_LEN, 100 / portTICK_PERIOD_MS);
    i2c_reset_rx_fifo(this->port);

    if (strlen((char *)received_data) == 0) {
        free(received_data);
        return NULL;
    }

    ESP_LOGI(TAG, "Data in I2C(%d): %s", this->sda_io, received_data);

    return (char *)received_data;
}

void I2CSlave::write_buffer(char *data) {
    i2c_slave_write_buffer(this->port, (uint8_t *)data, strlen(data), portMAX_DELAY);
}
