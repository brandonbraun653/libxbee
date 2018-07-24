#ifndef XBEE_CHIMERA_SERIAL_HPP
#define XBEE_CHIMERA_SERIAL_HPP

/* C/C++ Includes */
#include <stdlib.h>
#include <stdint.h>

/* LibXBEE Includes */
#include <libxbee/include/xb_serial.hpp>

/* Chimera Includes */
#include <Chimera/serial.hpp>

namespace libxbee
{
    class XbeeChimeraSerial : public XBEESerial 
    {
    public:

		void initialize(Chimera::Serial::BaudRate baud = Chimera::Serial::BaudRate::SERIAL_BAUD_115200,
			Chimera::Serial::Modes tx_mode = Chimera::Serial::Modes::BLOCKING,
			Chimera::Serial::Modes rx_mode = Chimera::Serial::Modes::BLOCKING);

        void write(uint8_t* data, size_t length) override;
        void read(uint8_t* data, size_t length) override;

		bool isInitialized();

		XbeeChimeraSerial(uint32_t channel);
		~XbeeChimeraSerial() = default;
    
	private:
		Chimera::Serial::SerialClass* serial;

		bool initialized = false;

	};
}


#endif /* !XBEE_CHIMERA_SERIAL_HPP */