#ifndef XBEE_PRO_SERIES_2_HPP
#define XBEE_PRO_SERIES_2_HPP

#include <libxbee/include/xb_serial.hpp>
#include <Chimera/gpio.hpp>

namespace libxbee
{
    namespace modules
    {
        class xbeeProS2
        {
            public:
				bool connect();

                void getDeviceMetaData();
                bool setSerialBaud();

				template<typename T>
				xbeeProS2(T* serialInterface, Chimera::GPIO::GPIOClass* rstPin)
				{
					serial = dynamic_cast<libxbee::XBEESerial*>(serialInterface);
					gpio_reset = rstPin;

					gpio_reset->mode(Chimera::GPIO::Mode::OUTPUT_PUSH_PULL);
					gpio_reset->write(Chimera::GPIO::State::HIGH);
				}

				~xbeeProS2() = default;

            private:
				libxbee::XBEESerial* serial;
				Chimera::GPIO::GPIOClass* gpio_reset;
		};
    }
}

#endif /* !XBEE_PRO_SERIES_2_HPP */