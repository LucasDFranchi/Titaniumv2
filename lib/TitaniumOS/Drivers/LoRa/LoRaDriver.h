#ifndef LORA_DRIVER_H
#define LORA_DRIVER_H

#include "Drivers/DriverInterface/ICommunicationDriver.h"
#include "HAL/spi/SPIManager.h"
#include "Application/error/error_enum.h"

namespace CRCMode {
    constexpr uint8_t ENABLE  = 0x00; /**< CRC enable mode. */
    constexpr uint8_t DISABLE = 0x01; /**< CRC disable mode. */
}  // namespace CRCMode

namespace Regions {
    constexpr uint32_t EUROPE    = 868e6; /**< Frequency region for Europe. */
    constexpr uint32_t US        = 915e6; /**< Frequency region for US. */
    constexpr uint32_t ASIA      = 920e6; /**< Frequency region for Asia. */
    constexpr uint32_t AUSTRALIA = 915e6; /**< Frequency region for Australia. */
    constexpr uint32_t CHINA     = 782e6; /**< Frequency region for China. */
    constexpr uint32_t INDIA     = 866e6; /**< Frequency region for India. */
    constexpr uint32_t KOREA     = 920e6; /**< Frequency region for Korea. */
    constexpr uint32_t JAPAN     = 920e6; /**< Frequency region for Japan. */
    constexpr uint32_t BRAZIL    = 915e6; /**< Frequency region for Brazil. */
}  // namespace Regions

/**
 * @brief Driver class for LoRa communication.
 *
 * This class implements methods to control and interact with a LoRa transceiver,
 * including initialization, configuration of operating parameters like frequency,
 * power, spreading factor, etc., and sending/receiving data packets.
 */
class LoRaDriver : public IDriverInterface {
   public:
    /**
     * @brief Constructor for LoRaDriver class.
     *
     * @param[in] freq_region Frequency region of the LoRa transceiver.
     * @param[in] crc_mode CRC mode (ENABLE or DISABLE).
     * @param[in] buffer_size Size of the buffer for data transmission/reception.
     */
    LoRaDriver(uint32_t freq_region, uint8_t crc_mode, uint16_t buffer_size) {
        this->_buffer_size = buffer_size;
        this->Initialize();
        this->SetFrequency(freq_region);
        this->SetCRCMode(crc_mode);
    };

    titan_err_t Write(uint8_t* raw_bytes, uint16_t size);
    uint16_t Read(uint8_t* raw_bytes);

    /**
     * @brief Retrieves the buffer size used for data operations.
     *
     * @return Size of the buffer.
     */
    uint16_t buffer_size() const {
        return this->_buffer_size;
    }

   private:
    titan_err_t Initialize(void);
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
    titan_err_t WriteRegister(uint8_t register_address, uint8_t register_value);
    uint8_t ReadRegister(uint8_t register_address);
    titan_err_t ValidateVersion(void);

    SPIManager* spi_manager   = nullptr; /**< Pointer to the SPI manager instance. */
    GPIOManager* gpio_manager = nullptr; /**< Pointer to the GPIO manager instance. */
    uint32_t _frequency       = 0;       /**< Frequency of the LoRa transceiver. */
    bool _implicit            = false;   /**< Flag indicating implicit header mode. */
    bool _reciever_mode       = false;   /**< Flag indicating receiver mode. */
    uint16_t _buffer_size     = 0;       /**< Size of the data buffer used for operations. */
};

#endif /* LORA_DRIVER_H */