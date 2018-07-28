#include <libxbee/include/modules/xbee_pro_s2/xbpros2.hpp>


using namespace Chimera::GPIO;
using namespace Chimera::Serial;

static const size_t XB_DEFAULT_TIMEOUT_mS = 10;
static const size_t XB_PING_TIMEOUT_mS = 2000;
static const size_t XB_ENTER_AT_TIMEOUT_mS = 2000;

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

				/* Reset is active low, so make sure this is high */
				reset->mode(Chimera::GPIO::Mode::OUTPUT_PUSH_PULL);
				reset->write(Chimera::GPIO::State::HIGH);

				/* Use this as a default starting point. Will likely change later */
				serial->begin(BaudRate::SERIAL_BAUD_9600, Modes::BLOCKING, Modes::INTERRUPT);

				/* Give the Xbee a little bit to stabilize from the possible reset event */
				Chimera::delayMilliseconds(500);
			}

			libxbee::XBStatus XBEEProS2::discover(Chimera::Serial::BaudRate baud)
			{
				

				
			}

			libxbee::XBStatus XBEEProS2::initialize(const Config& config)
			{

			}

			bool XBEEProS2::ping()
			{
				bool success = false;
				uint8_t result[2] = { 0, 0 };

				sendCommand(XB_FIRMWARE_VER, strlen(XB_FIRMWARE_VER));

				/* If we get any response at all, it means we are still in AT mode and we can respond to commands.
				 * At this point, it doesn't really matter what the actual response is.*/
				if (readResultWithTimeout(result, sizeof(result), XB_PING_TIMEOUT_mS) == XB_OK)
				{
					success = true;	
				}

				return success;
			}

			Version XBEEProS2::getVersion()
			{
				Version temp;
				memset(&temp, 0, sizeof(Version));

				

			}


			libxbee::XBStatus XBEEProS2::txCommandWithResult(uint8_t* dataIn, size_t lenIn, uint8_t* dataOut, size_t lenOut, size_t timeout)
			{
				XBStatus result = XB_OK;

				if ((dataIn == nullptr) || (dataOut == nullptr))
				{
					return XB_INVALID_ADDRESS;
				}

				/* Send the command sequence with the assumption we are still in command mode */
				sendCommand((const char*)dataIn, lenIn);
				result = readResultWithTimeout(dataOut, lenOut, timeout);

				switch (result)
				{
				case XBStatus::XB_OK:
					break;

				case XBStatus::XB_TIMEOUT:
					/* If we get here, the Xbee was not in command mode. Go back to AT mode and try again. */
					if (goToCommandMode() == XB_OK)
					{
						sendCommand((const char*)dataIn, lenIn);
						result = readResultWithTimeout(dataOut, lenOut, timeout);
					}
					else
					{
						result = XB_NO_RESPONSE;
					}
					break;

				//Add more error cases as they arrive

				default:
					result = XB_UNKNOWN_ERROR;
					break;
				}
				

				return result;
			}

			libxbee::XBStatus XBEEProS2::goToCommandMode()
			{
				XBStatus result = XB_NO_RESPONSE;
				uint8_t response[strlen(XB_AT_MODE_RESP)];
				memset(response, 0, sizeof(response));

				sendCommand(XB_ENTER_AT_MODE, strlen(XB_ENTER_AT_MODE));

				if (readResultWithTimeout(response, sizeof(response), XB_ENTER_AT_TIMEOUT_mS) == XB_OK)
				{
					if (memcmp(response, XB_AT_MODE_RESP, sizeof(response)) == 0)
					{
						result = XB_OK;
					}
					else
					{
						result = XB_BAD_RESPONSE;
					}
				}

				return result;
			}

			void XBEEProS2::sendCommand(const char* data, size_t length)
			{
				serial->write(data, length);
			}

			XBStatus XBEEProS2::readResultWithTimeout(uint8_t* data, size_t length, size_t timeout_mS)
			{
				XBStatus result = XB_TIMEOUT;
				size_t startTime = 0;
				size_t recheckDelay_mS = 10;	

				while ((startTime < timeout_mS) || (result == XB_TIMEOUT))
				{
					if (serial->availablePackets())
					{
						auto error = serial->readPacket(data, length);

						switch (error)
						{
						case Chimera::Serial::Status::SERIAL_OK:
							result = XB_OK;
							break;

						case Chimera::Serial::Status::SERIAL_PACKET_TOO_LARGE_FOR_BUFFER:
							result = XB_BUFFER_OVERRUN;
							break;

						default:
							result = XB_UNKNOWN_ERROR;
							break;
						}
					}
					else
					{
						Chimera::delayMilliseconds(recheckDelay_mS);
						startTime += recheckDelay_mS;
					}
				}

				return result;
			}

		}
		
	}
}
