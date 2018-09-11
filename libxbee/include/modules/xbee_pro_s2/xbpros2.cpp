/* C/C++ Includes */
#include <string>

/* Chimera Includes */
#include <Chimera/logging.hpp>


#include <libxbee/include/modules/xbee_pro_s2/xbpros2.hpp>


using namespace Chimera::GPIO;
using namespace Chimera::Serial;
using namespace Chimera::Logging;

static const size_t XB_DEFAULT_TIMEOUT_mS = 10;


static const size_t XB_SEMPHR_TAKE_TIMEOUT_mS = 100;


namespace libxbee
{
	namespace modules
	{
		namespace XBEEProS2
		{
			XBEEProS2::XBEEProS2(int serialChannel, Chimera::GPIO::Port rstPort, uint8_t rst_pin)
			{
				txComplete = xSemaphoreCreateCounting(32, 0);
				rxComplete = xSemaphoreCreateBinary();
				txRxComplete = xSemaphoreCreateBinary();

				serial = new SerialClass(serialChannel);
				reset = new GPIOClass(rstPort, rst_pin);

				/* Reset is active low, so make sure the pin is high on startup */
				reset->mode(Chimera::GPIO::Mode::OUTPUT_PUSH_PULL);
				reset->write(Chimera::GPIO::State::HIGH);

				/* Use this as a default starting point. Will likely change later in user code */
				serial->begin(BaudRate::SERIAL_BAUD_115200, Modes::BLOCKING, Modes::INTERRUPT);
				serial->attachThreadTrigger(TX_COMPLETE, &txComplete);
				serial->attachThreadTrigger(RX_COMPLETE, &rxComplete);
				serial->attachThreadTrigger(TXRX_COMPLETE, &txRxComplete);

				/* Give the Xbee a little bit to stabilize from the possible reset event */
				Chimera::delayMilliseconds(500);
			}

            XBEEProS2::~XBEEProS2()
            {
            }

            libxbee::XBStatus XBEEProS2::discover(uint32_t baud)
			{
				Console.log(Level::INFO, "Starting XBEE Discovery...\r\n");
				uint32_t discoveryBaud = baud;
				XBStatus discoveryResult = XB_NOT_FOUND;

				serial->setBaud(baud);

				/* First try to ping the device at the requested baudrate. If that fails, iterate through
				 * the standard baudrates, hoping that it lies on one of them. */
				if (goToCommandMode() != XB_OK)
				{
					uint32_t standardRates[] = { 9600u, 19200u, 38400u, 57600u, 115200u, 230400u, 460800u, 921600u };

					for (size_t i = 0; i < sizeof(standardRates); i++)
					{
						Console.log(Level::INFO, "Pinging XBEE at baud: %d\r\n", standardRates[i]);
						serial->setBaud(standardRates[i]);
						
						if (goToCommandMode() == XB_OK)
						{
							discoveryBaud = standardRates[i];
							discoveryResult = XB_OK;
                            Console.log(Level::INFO, "Found XBEE at baud: %d\r\n", discoveryBaud);
							break;
						}
					}

                    /* Print out some statements depending on the result of discovery */
                    #ifdef DEBUG
                    if ((discoveryResult == XB_OK) && (discoveryBaud != baud))
                    {
                        Console.log(Level::WARN, "Discovered baud rate [%d] doesn't match expected [%d]\r\n", discoveryBaud, baud);
                    }
                    else
                    {
                        Console.log(Level::WARN, "XBEE not found with standard baud rates at 8N1.\r\n");
                    }
                    #endif
				}
                else
                {
                    discoveryResult = XB_OK;
                    Console.log(Level::INFO, "Found XBEE at baud: %d\r\n", discoveryBaud);
                }
				
				return discoveryResult;
			}

			libxbee::XBStatus XBEEProS2::initialize(const Config& config)
			{   
                if (!updateTimingParams())
                {
                    #ifdef DEBUG
                    Console.log(Level::ERROR, "XBEE: Failed updating timing info!\r\n");
                    #endif

                    return XB_BAD_RESULT;
                }

                /* First change the baud rate to the desired value and make sure we can talk */
                

                /* Then update the rest of the parameters. */

                size_t result = setATModeTimeout(6000, true);
                Console.log(Level::INFO, "Set to [%d]\r\n", result);

                /* keep the setting changes here minimal. We want the user to take advantage of the API 
                 * and build their own functions to write commands. 
                 */

                /* Finally, apply all the changes */

                return XB_OK;
			}

            libxbee::XBStatus XBEEProS2::applyChanges()
            {
                txFrameWithResult(XB_APPLY_CHANGES);

                if (isRxBufferEqual(XB_DEFAULT_RESPONSE))
                {
                    return XB_OK;
                } 
                else
                {
                    return XB_FAILED_COMMAND;
                }
            }

            libxbee::XBStatus XBEEProS2::updateNonVolatileMemory()
            {
                txFrameWithResult(XB_WRITE_MEMORY);

                if (isRxBufferEqual(XB_DEFAULT_RESPONSE))
                {
                    return XB_OK;
                }
                else
                {
                    return XB_FAILED_COMMAND;
                }
            }

            void XBEEProS2::setBaudRate(uint32_t baud, bool applyChange)
            {
                // Change baud cmd, apply change, reconfig uart after OK\r, then try to enter cmd mode
            }

            uint32_t XBEEProS2::getBuadRate()
            {

            }

            size_t XBEEProS2::setATModeTimeout(size_t atTimeout_mS, bool verify)
            {
                uint16_t registerValue = XB_MAX_AT_TIMEOUT_HEX;

                /* Some quick boundary checking... */
                if (atTimeout_mS <= XB_MIN_AT_TIMEOUT_MS)
                {
                    registerValue = XB_MIN_AT_TIMEOUT_HEX;
                }
                else if (atTimeout_mS >= XB_MAX_AT_TIMEOUT_MS)
                {
                    registerValue = XB_MAX_AT_TIMEOUT_HEX;
                }
                else
                {
                    registerValue = atTimeout_mS / XB_AT_TIMEOUT_MULT;
                }

                /* Make sure the OK response is captured before moving on */
                txFrameWithResult(XB_CMD_MODE_TIMEOUT, registerValue);

                if (verify)
                {
                    verifyParameter(XB_CMD_MODE_TIMEOUT, registerValue);

                    /* Convert the actual value back into milliseconds. The result of verifyParameter
                     * is stored in the rxBuffer as a hex string. */
                    strip(rxBuffer, "\r\n");
                    uint16_t tmp = (uint16_t)strtol(rxBuffer, nullptr, 16);
                    return (size_t)(tmp * XB_AT_TIMEOUT_MULT);
                }
                else
                {
                    return atTimeout_mS;
                }
            }

            size_t guardTime(size_t guardTime_mS, bool verify)
            {
                /* First write the data to the XBEE */

                /* If needed, verify that the expected value matches the actual */
            }

            bool XBEEProS2::isATMode()
            {
                /* Easiest way to check is if we can get the version string */
                bool success = false;

                #ifdef USING_FREERTOS
                if ((xTaskGetTickCount() - lastCmdMode) < atModeTimeout_mS)
                {
                    success = true;
                }
                #else
                /* If we get any response at all, it means we are in AT mode...for now.*/
                uint16_t dummyVar;
                write(XB_FIRMWARE_VER, strlen(XB_FIRMWARE_VER));
                if (readWithTimeout((uint8_t*)&dummyVar, sizeof(dummyVar), guardTimeout_mS) == XB_OK)
                {
                    success = true;
                }
                #endif
               
                return success;
            }


            libxbee::XBStatus XBEEProS2::goToCommandMode()
			{
				XBStatus result = XB_NO_RESPONSE;
				memset(rxBuffer, 0, XBEE_RX_BUFFER_SIZE);

				write(XB_ENTER_AT_MODE, strlen(XB_ENTER_AT_MODE));

				if (readWithTimeout((uint8_t*)rxBuffer, XBEE_RX_BUFFER_SIZE, XB_ENTER_AT_TIMEOUT_mS) == XB_OK)
				{
					if (memcmp(rxBuffer, XB_DEFAULT_RESPONSE, sizeof(XB_DEFAULT_RESPONSE)) == 0)
					{
						result = XB_OK;
                        Chimera::delayMilliseconds(guardTimeout_mS);

                        /* Keep track of the tick at which we entered AT mode. This helps later with determining if 
                         * we have timed out of AT mode.*/
                        #ifdef USING_FREERTOS
                        lastCmdMode = xTaskGetTickCount();
                        #endif
					}
					else
					{
						result = XB_BAD_RESPONSE;
					}
				}

                #ifdef DEBUG
                if (result != XB_OK)
                {
                    Console.log(Level::ERROR, "XBEE: Failed to enter AT mode!\r\n");
                }
                #endif

				return result;
			}

			libxbee::XBStatus XBEEProS2::readWithTimeout(uint8_t* data, size_t length, size_t timeout_mS)
			{
				XBStatus result = XB_TIMEOUT;
				size_t startTime = 0;
				size_t recheckDelay_mS = 10;	

				while (startTime < timeout_mS)
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

						break;
					}
					else
					{
						Chimera::delayMilliseconds(recheckDelay_mS);
						startTime += recheckDelay_mS;
					}
				}

				return result;
			}


            bool XBEEProS2::updateTimingParams()
            {
                #ifdef DEBUG
                Console.log(Level::INFO, "XBEE: Initializing timing info\r\n");
                #endif

                if (!isATMode() && (goToCommandMode() != XB_OK))
                {
                    return false;
                }

                int bytes = 0;
                bool updateSuccess = true;

                /* Guard Time */
                memset(rxBuffer, 0, XBEE_RX_BUFFER_SIZE);
                bytes = frameBuilder(XB_SET_GUARD_TIME, nullptr);

                if (bytes > 0)
                {
                    write(txBuffer, (size_t)bytes);
                    if (readWithTimeout((uint8_t*)rxBuffer, XBEE_RX_BUFFER_SIZE, XB_DEFAULT_TIMEOUT_mS) == XB_OK)
                    {
                        guardTimeout_mS = (uint16_t)strtol(rxBuffer, nullptr, 16);
                    }
                    else
                    {
                        updateSuccess = false;
                    }
                }
                

                /* AT Command Timeout */
                memset(rxBuffer, 0, XBEE_RX_BUFFER_SIZE);
                bytes = frameBuilder(XB_CMD_MODE_TIMEOUT, nullptr);

                if (bytes > 0)
                {
                    write(txBuffer, (size_t)bytes);
                    if (readWithTimeout((uint8_t*)rxBuffer, XBEE_RX_BUFFER_SIZE, XB_DEFAULT_TIMEOUT_mS) == XB_OK)
                    {
                        atModeTimeout_mS = ((uint16_t)strtol(rxBuffer, nullptr, 16)) * 100;
                    }
                    else
                    {
                        updateSuccess = false;
                    }
                }

                return updateSuccess;
            }

            char* XBEEProS2::strip(char* buffer, const char* chars)
            {
                /* Thanks SOF! https://stackoverflow.com/questions/2693776/removing-trailing-newline-character-from-fgets-input */
                buffer[strcspn(buffer, chars)] = 0;
            }

            template<>
            XBStatus XBEEProS2::verifyParameter(const char* command, const char* expected)
            {
                if (!command)
                {
                    return XB_INVALID_PARAM;
                }

                XBStatus result = txFrameWithResult(command);

                if (result == XB_OK && (memcmp(rxBuffer, expected, strlen(expected)) != 0))
                {
                    result = XB_FAILED_COMPARE;

                    #ifdef DEBUG
                    Console.log(Level::ERROR, "XBEE: Param compare [%s] doesn't match [%s]", rxBuffer, txBuffer);
                    #endif
                }

                return result;
            }

            bool XBEEProS2::isRxBufferEqual(const char* data)
            {
                bool result = true;

                if (memcmp(rxBuffer, data, strlen(data)) != 0)
                {
                    result = false;
                }

                return result; 
            }

        }
		
	}
}
