
#include "esp_err.h"

#include "SPIManager.h"

namespace CRCMode {
    constexpr uint8_t ENABLE = 0x00;
    constexpr uint8_t DISABLE = 0x01;
}

/**
 * Driver used to LoRa communication.
 */
class LoRaDriver{
    public:
    LoRaDriver(){};

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
    void SendPacket(uint8_t *pOut, uint8_t size);
    uint8_t ReceivePacket(uint8_t *pIn, uint8_t size);

    private:
    esp_err_t WriteRegister(uint8_t register_address, uint8_t register_value);
    uint32_t  ReadRegister(uint8_t register_address);
    esp_err_t ValidateVersion(void);

    SPIManager*  spi_manager = nullptr;
    GPIOManager* gpio_manager = nullptr;

    uint32_t frequency_ = 0;
    bool implicit_ = false;
    bool reciever_mode_ = false;
};
