#ifndef LORA_DRIVER_TEMPLATE_GUARD
#define LORA_DRIVER_TEMPLATE_GUARD

#include "HAL/spi/SPIManager.h"

#include "esp_err.h"

#include "Drivers/DriverInterface/ICommunicationDriver.h"

namespace CRCMode {
    constexpr uint8_t ENABLE  = 0x00;
    constexpr uint8_t DISABLE = 0x01;
}  // namespace CRCMode

namespace Regions {
    constexpr uint32_t EUROPE    = 868e6;
    constexpr uint32_t US        = 915e6;
    constexpr uint32_t ASIA      = 920e6;
    constexpr uint32_t AUSTRALIA = 915e6;
    constexpr uint32_t CHINA     = 782e6;
    constexpr uint32_t INDIA     = 866e6;
    constexpr uint32_t KOREA     = 920e6;
    constexpr uint32_t JAPAN     = 920e6;
    constexpr uint32_t BRAZIL    = 915e6;
}  // namespace Regions

/**
 * Driver used to LoRa communication.
 */
class LoRaDriver : public IDriverInterface {
   public:
    LoRaDriver(uint32_t freq_region, uint8_t crc_mode, uint16_t buffer_size) {
        this->_buffer_size = buffer_size;
        this->Initialize();
        this->SetFrequency(freq_region);
        this->SetCRCMode(crc_mode);
    };

    esp_err_t Write(uint8_t* raw_bytes, uint16_t size);
    uint16_t Read(uint8_t* raw_bytes);

    uint16_t buffer_size() const {
        return this->_buffer_size;
    }

   private:
    esp_err_t Initialize(void);
    void Reset(void);

    void SetExplicitHeaderMode(void);
    void SetImplicitHeaderMode(uint8_t size);
    void SetIdleMode(void);
    void SetSleepMode(void);
    void SetReceiverMode(void);
    void SetTxPower(uint8_t tx_power);
    void SetFrequency(uint32_t frequency);
    void SetSpreadingFactor(uint8_t spreading_factor);
    void SetBandwidth(uint32_t bandwidth);
    void SetCodingRate(uint8_t denominator);
    void SetPreambleLength(uint32_t length);
    void SetSyncWord(uint8_t sync_word);
    void SetCRCMode(uint8_t mode);

    uint32_t GetLastPacketRSSI(void);
    uint8_t GetLastPacket4TSNR(void);

    bool isDataInReceiver(void);
    void SendPacket(uint8_t* pOut, uint8_t size);
    uint8_t ReceivePacket(uint8_t* pIn, uint8_t size);
    esp_err_t WriteRegister(uint8_t register_address, uint8_t register_value);
    uint32_t ReadRegister(uint8_t register_address);
    esp_err_t ValidateVersion(void);

    SPIManager* spi_manager   = nullptr;
    GPIOManager* gpio_manager = nullptr;

    uint32_t _frequency   = 0;
    bool _implicit        = false;
    bool _reciever_mode   = false;
    uint16_t _buffer_size = 0;
};

#endif /* LORA_DRIVER_TEMPLATE_GUARD */