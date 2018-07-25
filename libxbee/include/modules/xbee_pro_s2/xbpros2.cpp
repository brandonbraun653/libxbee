#include <libxbee/include/modules/xbee_pro_s2/xbpros2.hpp>


using namespace Chimera::GPIO;
using namespace Chimera::Serial;

static const uint32_t recheckDelay = 10;

namespace libxbee
{
	namespace modules
	{
		namespace XBEEProS2
		{
			XBEEProS2::XBEEProS2(int serialChannel, Chimera::GPIO::Port rstPort, uint8_t rst_pin)
			{
				serial = new SerialClass(serialChannel);
				reset = new GPIOClass(rstPort, rst_pin);


			}

			void XBEEProS2::initialize(Chimera::Serial::BaudRate baud)
			{
				/* Reset is active low */
				reset->mode(Chimera::GPIO::Mode::OUTPUT_PUSH_PULL);
				reset->write(Chimera::GPIO::State::HIGH);

				serial->begin(baud, Modes::BLOCKING, Modes::INTERRUPT);

				/* Allow the system to set itself up */
				Chimera::delayMilliseconds(2000);
			}

			bool XBEEProS2::ping()
			{
				bool success = false;
				uint8_t result[strlen(XB_EXP_RSLT_PING)];

				sendCMD(XB_ENTER_AT_MODE, strlen(XB_ENTER_AT_MODE));

				if (readResultWithTimeout(result, strlen(XB_EXP_RSLT_PING), 2000))
				{
					if (memcmp(result, XB_EXP_RSLT_PING, strlen(XB_EXP_RSLT_PING)) == 0)
						success = true;
				}

				return success;
			}



			void XBEEProS2::sendCMD(const char* data, size_t length)
			{
				serial->write(data, length);
			}

			bool XBEEProS2::readResultWithTimeout(uint8_t* data, size_t length, uint32_t timeout)
			{
				bool success = false;
				uint32_t startTime = 0;

				while (startTime < timeout)
				{
					if (serial->availablePackets())
					{
						serial->readPacket(data, length);
						success = true;
						break;
					}

					Chimera::delayMilliseconds(recheckDelay);
					startTime += recheckDelay;
				}

				return success;
			}

		}
		
	}
}
