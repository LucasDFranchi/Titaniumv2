#include "LoRaDriver.h"

#include "GPIOManager.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"

namespace Registers {
    constexpr uint8_t FIFO = 0x00;
    constexpr uint8_t OP_MODE = 0x01;
    constexpr uint8_t FRF_MSB = 0x06;
    constexpr uint8_t FRF_MID = 0x07;
    constexpr uint8_t FRF_LSB = 0x08;
    constexpr uint8_t PA_CONFIG = 0x09;
    constexpr uint8_t LNA = 0x0c;
    constexpr uint8_t FIFO_ADDR_PTR = 0x0d;
    constexpr uint8_t FIFO_TX_BASE_ADDR = 0x0e;
    constexpr uint8_t FIFO_RX_BASE_ADDR = 0x0f;
    constexpr uint8_t FIFO_RX_CURRENT_ADDR = 0x10;
    constexpr uint8_t IRQ_FLAGS = 0x12;
    constexpr uint8_t RX_NB_BYTES = 0x13;
    constexpr uint8_t PKT_SNR_VALUE = 0x19;
    constexpr uint8_t PKT_RSSI_VALUE = 0x1a;
    constexpr uint8_t MODEM_CONFIG_1 = 0x1d;
    constexpr uint8_t MODEM_CONFIG_2 = 0x1e;
    constexpr uint8_t PREAMBLE_MSB = 0x20;
    constexpr uint8_t PREAMBLE_LSB = 0x21;
    constexpr uint8_t PAYLOAD_LENGTH = 0x22;
    constexpr uint8_t MODEM_CONFIG_3 = 0x26;
    constexpr uint8_t RSSI_WIDEBAND = 0x2c;
    constexpr uint8_t DETECTION_OPTIMIZE = 0x31;
    constexpr uint8_t DETECTION_THRESHOLD = 0x37;
    constexpr uint8_t SYNC_WORD = 0x39;
    constexpr uint8_t DIO_MAPPING_1 = 0x40;
    constexpr uint8_t VERSION = 0x42;
}

namespace TransceiverModes {
    constexpr uint8_t LONG_RANGE_MODE = 0x80;
    constexpr uint8_t SLEEP = 0x00;
    constexpr uint8_t STDBY = 0x01;
    constexpr uint8_t TX = 0x03;
    constexpr uint8_t RX_CONTINUOUS = 0x05;
    constexpr uint8_t RX_SINGLE = 0x06;
}

namespace PAConfiguration {
    constexpr uint8_t PA_BOOST = 0x80;
    constexpr uint8_t PA_OUTPUT_RFO_PIN = 0x80;
    constexpr uint8_t PA_OUTPUT_PA_BOOST_PIN = 0x80;
}

namespace IRQ {
    constexpr uint8_t TX_DONE_MASK = 0x08;
    constexpr uint8_t PAYLOAD_CRC_ERROR_MASK = 0x20;
    constexpr uint8_t RX_DONE_MASK = 0x40;
}

namespace Driver{
    constexpr uint8_t VERSION = 0x12;
    constexpr uint8_t WRITE_COMMAND = 0x80;
}

esp_err_t LoRaDriver::Initialize(void){
    auto result = ESP_OK;

    this->spi_manager = SPIManager::GetInstance();
    this->gpio_manager = GPIOManager::GetInstance();

    this->Reset();

    result += this->ValidateVersion();

    this->SetSleepMode();
    result += this->WriteRegister(Registers::FIFO_RX_BASE_ADDR, 0);
    result += this->WriteRegister(Registers::FIFO_TX_BASE_ADDR, 0);
    result += this->WriteRegister(Registers::LNA, this->ReadRegister(Registers::LNA) | 0x03);
    result += this->WriteRegister(Registers::MODEM_CONFIG_3, 0x04);
    this->SetTxPower(17);

    this->SetIdleMode();

    ESP_ERROR_CHECK(result);

    return result;
}

void LoRaDriver::Reset(void){
    gpio_manager->WriteGPIO(LORA_RST, LOW);
    vTaskDelay(pdMS_TO_TICKS(10));
    gpio_manager->WriteGPIO(LORA_RST, HIGH);
    vTaskDelay(pdMS_TO_TICKS(10));
}

void LoRaDriver::SetExplicitHeaderMode(void){
    this->implicit_ = false;
    this->WriteRegister(Registers::MODEM_CONFIG_1, this->ReadRegister(Registers::MODEM_CONFIG_1) & 0xfe);
}

void LoRaDriver::SetImplicitHeaderMode(uint8_t size)
{
   this->implicit_ = true;
   this->WriteRegister(Registers::MODEM_CONFIG_1, this->ReadRegister(Registers::MODEM_CONFIG_1) | 0x01);
   this->WriteRegister(Registers::PAYLOAD_LENGTH, size);
}

void LoRaDriver::SetSleepMode(void){ 
   this->WriteRegister(Registers::OP_MODE, TransceiverModes::LONG_RANGE_MODE | TransceiverModes::SLEEP);
}

void LoRaDriver::SetIdleMode(void){ 
   this->WriteRegister(Registers::OP_MODE, TransceiverModes::LONG_RANGE_MODE | TransceiverModes::STDBY);
}

void LoRaDriver::SetReceiverMode(void)
{
    this->WriteRegister(Registers::OP_MODE, TransceiverModes::LONG_RANGE_MODE | TransceiverModes::RX_CONTINUOUS);
    if (!this->reciever_mode_){
        this->reciever_mode_ = true;
    }
}

void LoRaDriver::SetTxPower(uint8_t level)
{
    if (level < 2){
        level = 2;
    }
    else if (level > 17) {
        level = 17;
    }
   this->WriteRegister(Registers::PA_CONFIG, PAConfiguration::PA_BOOST | (level - 2));
}

void LoRaDriver::SetFrequency(uint32_t frequency)
{
   this->frequency_ = frequency;

   uint64_t frf = ((uint64_t)frequency << 19) / 32000000;

   this->WriteRegister(Registers::FRF_MSB, (uint8_t)(frf >> 16));
   this->WriteRegister(Registers::FRF_MID, (uint8_t)(frf >> 8));
   this->WriteRegister(Registers::FRF_LSB, (uint8_t)(frf >> 0));
}

void LoRaDriver::SetSpreadingFactor(uint8_t spreading_factor)
{
    if (spreading_factor < 6) {
        spreading_factor = 6;
    } 
    else if (spreading_factor > 12) {
        spreading_factor = 12;
    }

    if (spreading_factor == 6) {
        this->WriteRegister(Registers::DETECTION_OPTIMIZE, 0xc5);
        this->WriteRegister(Registers::DETECTION_THRESHOLD, 0x0c);
    } else {
        this->WriteRegister(Registers::DETECTION_OPTIMIZE, 0xc3);
        this->WriteRegister(Registers::DETECTION_THRESHOLD, 0x0a);
    }

   this->WriteRegister(Registers::MODEM_CONFIG_2, (this->ReadRegister(Registers::MODEM_CONFIG_2) & 0x0f) | ((spreading_factor << 4) & 0xf0));
}

void LoRaDriver::SetBandwidth(uint32_t bandwidth)
{
    uint32_t bw;

    if (bandwidth <= 7.8E3) {
        bw = 0;
    } 
    else if (bandwidth <= 10.4E3) {
        bw = 1;
    }
    else if (bandwidth <= 15.6E3) {
        bw = 2;
    } 
    else if (bandwidth <= 20.8E3) {
        bw = 3;
    }
    else if (bandwidth <= 31.25E3) {
        bw = 4;
    }
    else if (bandwidth <= 41.7E3) {
        bw = 5;
    }
    else if (bandwidth <= 62.5E3) {
        bw = 6;
    } 
    else if (bandwidth <= 125E3) { 
        bw = 7;
    } 
    else if (bandwidth <= 250E3) {
        bw = 8;
    }
    else {
        bw = 9;
    }
    this->WriteRegister(Registers::MODEM_CONFIG_1, (this->ReadRegister(Registers::MODEM_CONFIG_1) & 0x0f) | (bw << 4));
}

void LoRaDriver::SetCodingRate(uint8_t denominator)
{
    if (denominator < 5) {
        denominator = 5;
    }
    else if (denominator > 8) {
        denominator = 8;
    }

   int cr = denominator - 4;
   this->WriteRegister(Registers::MODEM_CONFIG_1, (this->ReadRegister(Registers::MODEM_CONFIG_1) & 0xf1) | (cr << 1));
}

void LoRaDriver::SetPreambleLength(uint32_t length)
{
   this->WriteRegister(Registers::PREAMBLE_MSB, (uint8_t)(length >> 8));
   this->WriteRegister(Registers::PREAMBLE_LSB, (uint8_t)(length >> 0));
}

void LoRaDriver::SetSyncWord(uint8_t sync_word){
   this->WriteRegister(Registers::SYNC_WORD, sync_word);
}

void LoRaDriver::SetCRCMode(uint8_t mode){
    if (mode == CRCMode::ENABLE){
        this->WriteRegister(Registers::MODEM_CONFIG_2, this->ReadRegister(Registers::MODEM_CONFIG_2) | 0x04);
    } 
    else {
        this->WriteRegister(Registers::MODEM_CONFIG_2, this->ReadRegister(Registers::MODEM_CONFIG_2) & 0xfb);
    }
}

void LoRaDriver::SendPacket(uint8_t *pOut, uint8_t size) {

    this->SetIdleMode();
    this->WriteRegister(Registers::FIFO_ADDR_PTR, 0);

    for (uint8_t i = 0; i < size; i++) 
        this->WriteRegister(Registers::FIFO, pOut[i]);
    
    this->WriteRegister(Registers::PAYLOAD_LENGTH, size);
    this->WriteRegister(Registers::OP_MODE, TransceiverModes::LONG_RANGE_MODE |  TransceiverModes::TX);
    while((this->ReadRegister(Registers::IRQ_FLAGS) & IRQ::TX_DONE_MASK) == ESP_OK)
        vTaskDelay(2);
    this->reciever_mode_ = true;

    this->WriteRegister(Registers::IRQ_FLAGS, IRQ::TX_DONE_MASK);
}

uint8_t LoRaDriver::ReceivePacket(uint8_t *pIn, uint8_t size) {
    uint8_t len = 0;

    uint8_t irq = this->ReadRegister(Registers::IRQ_FLAGS);
    this->WriteRegister(Registers::IRQ_FLAGS, irq);

    do{
        if((irq & IRQ::RX_DONE_MASK) == 0) {
            break;
        }
        if(irq & IRQ::PAYLOAD_CRC_ERROR_MASK) {
            break;
        }

        uint8_t len_register = this->implicit_ ? Registers::PAYLOAD_LENGTH : Registers::RX_NB_BYTES;

        uint8_t received_len = this->ReadRegister(len_register);
        len = received_len > size ? size : received_len;

        for (uint8_t i = 0; i < len; i++) {
            pIn[i] = this->ReadRegister(Registers::FIFO);
        }

    }while(0);
    
    return len;
}

bool LoRaDriver::isDataInReceiver(void){
    if(this->ReadRegister(Registers::IRQ_FLAGS) & IRQ::RX_DONE_MASK)
        return true;
    return false;
}

uint32_t LoRaDriver::GetLastPacketRSSI(void){
   return (this->ReadRegister(Registers::PKT_RSSI_VALUE) - (this->frequency_ < 868E6 ? 164 : 157));
}

uint8_t LoRaDriver::GetLastPacket4TSNR(void){
   return ((int8_t)this->ReadRegister(Registers::PKT_SNR_VALUE));
}

esp_err_t LoRaDriver::WriteRegister(uint8_t register_address, uint8_t register_value){
    uint8_t write_address = Driver::WRITE_COMMAND | register_address;
    uint8_t out[2] = { write_address, register_value};
    uint8_t in[2];

    return spi_manager->DeviceTransmit(out, in, sizeof(out));
}

uint32_t  LoRaDriver::ReadRegister(uint8_t register_address){
    uint8_t out[2] = {register_address, 0xff };
    uint8_t in[2] = {0};

    spi_manager->DeviceTransmit(out, in, sizeof(out));

    return in[1];
}

esp_err_t LoRaDriver::ValidateVersion(void){
    auto result = ESP_FAIL;
    auto max_retires = 5;

    for (uint8_t i = 0; i < max_retires; i++){
        uint8_t read_value = this->ReadRegister(Registers::VERSION);

        if (read_value == Driver::VERSION){
            result = ESP_OK;
            break;
        }
    }

    return result;
}
