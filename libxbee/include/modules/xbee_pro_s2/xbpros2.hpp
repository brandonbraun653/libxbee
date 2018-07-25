#ifndef XBEE_PRO_SERIES_2_HPP
#define XBEE_PRO_SERIES_2_HPP

/* Chimera Includes */
#include <Chimera/gpio.hpp>
#include <Chimera/serial.hpp>

/* Libxbee Includes */
#include <libxbee/include/xb_serial.hpp>
#include <libxbee/include/xb_definitions.hpp>

namespace libxbee
{
    namespace modules
    {
		namespace XBEEProS2
		{
			class XBEEProS2
			{
			public:
				void initialize(Chimera::Serial::BaudRate baud);

				bool ping();

				void getDeviceMetaData();
				bool setSerialBaud();

				XBEEProS2(int serialChannel, Chimera::GPIO::Port rstPort, uint8_t rstPin);
				~XBEEProS2() = default;

			private:
				Chimera::Serial::SerialClass* serial;
				Chimera::GPIO::GPIOClass* reset;

				void sendCMD(const char* data, size_t length);

				bool readResultWithTimeout(uint8_t* data, size_t length, uint32_t timeout);
			};
		}
        
    }
}

#endif /* !XBEE_PRO_SERIES_2_HPP */