#ifndef XBEE_PRO_SERIES_2_HPP
#define XBEE_PRO_SERIES_2_HPP

/* Chimera Includes */
#include <Chimera/gpio.hpp>
#include <Chimera/serial.hpp>
#include <Chimera/threading.hpp>

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
                static const size_t XBEE_TX_BUFFER_SIZE = 24;
                static const size_t XBEE_RX_BUFFER_SIZE = 24;
                static const size_t XB_ENTER_AT_TIMEOUT_mS = 2000;
                static const size_t XB_PING_TIMEOUT_mS = 2000;
                static const size_t XB_DEFAULT_TIMEOUT_mS = 100;

				/** Discovery of the Xbee
				 *	Attempts to connect to the Xbee and reconfigure it to desired baud rate. This is under the assumption
				 *	that the device's serial port is configured as 8N1. If connection cannot be established at the given baud
				 *	rate, the software will search through all available standard baud rates for the device, error-ing out if 
				 *	it cannot be found.
				 *
				 *	@param[in]	baud	Desired baud rate to use for communication
				 *	@return XB_OK if device was found, XB_NOT_FOUND if not
				 **/
				XBStatus discover(uint32_t baud);

				/** Initializes an Xbee after discovery has been performed
				 *	Attempts to reprogram Xbee settings according to a configuration struct. All settings are verified by
				 *	reading back of the data after writing.
				 *
				 *	@param[in] config	        Configuration struct for how the Xbee should be set up
				 *	@return                     XB_OK if everything is alright, error code from XBStatus if not
				 **/
				XBStatus initialize(const Config& config);

                /** Set/Read the period of inactivity (no valid commands received) after which the RF module automatically exits 
                 *  AT Command Mode and returns to Idle Mode.
                 *  
                 *  @param[in]  timeout_mS      How many milliseconds to set the timeout to. If left 0, 
                 *  @return     void
                 */
                size_t setATModeTimeout(size_t atTimeout_mS = 0, bool verify = false);

                /** Set required period of silence before and after the Command Sequence Characters of the AT Command Mode Sequence 
                 *  (GT + CC + GT). The period of silence is used to prevent inadvertent entrance into AT Command Mode.
                 *  
                 *  @param[in]  guardTime_mS    Length of the required silence period in milliseconds
                 *  @param[in]  verify          Checks with the XBEE to make sure it reports back the value just set
                 *  @return     uint16_t        If verify is true, returns back the value reported from the XBEE. Otherwise returns the input.
                 */
                size_t guardTime(size_t guardTime_mS = 0, bool verify = false);

                /** Checks if the device is in AT mode or not 
                 *  @return true if in AT mode, false if not */
                bool isATMode();


				XBStatus goToCommandMode();

				XBEEProS2(int serialChannel, Chimera::GPIO::Port rstPort, uint8_t rstPin);
				~XBEEProS2();


                size_t guardTimeout_mS = 1000;
                size_t atModeTimeout_mS = 5000;
            

			private:
				Chimera::Serial::SerialClass* serial;
				Chimera::GPIO::GPIOClass* reset;

				bool device_attached = false;
				
                TickType_t lastCmdMode;
				SemaphoreHandle_t txComplete;
				SemaphoreHandle_t rxComplete;
				SemaphoreHandle_t txRxComplete;

                template<typename T>
                void write(T* data, size_t length)
                {
                    /* Make sure that the low level write function can infer type! */
                    serial->write(data, length);
                }

				XBStatus readWithTimeout(uint8_t* data, size_t length, size_t timeout_mS);


                char txBuffer[XBEE_TX_BUFFER_SIZE];
                char rxBuffer[XBEE_RX_BUFFER_SIZE];

                bool updateTimingParams();

                char* strip(char* buffer, const char* chars);

                /** Frame Builder
                 *  Generates a frame of data, written to the internal tx buffer, from a given command and optional payload
                 *  
                 *	@param[in]	command     The command to be used
                 *  @param[in]  payload     If desired, data to be attached to the frame
                 *	@return     int         The number of bytes written to the internal tx buffer
                 **/
                template<typename T = uint16_t>
                int frameBuilder(const char* command, T payload = 0)
                {
                    if (!command)
                    {
                        return 0;
                    }

                    memset(txBuffer, 0, XBEE_TX_BUFFER_SIZE);

                    /* Write one of two formats depending on if a payload is present */
                    int bytesWritten = 0;
                    if (payload)
                    {
                        /* We have a payload and are writing a register on the XBEE */
                        bytesWritten = snprintf(txBuffer, XBEE_TX_BUFFER_SIZE, "%s %x%s", command, payload, XB_DELIMITER);
                    }
                    else
                    {
                        /* No payload. We are requesting data or acknowledging something. */
                        bytesWritten = snprintf(txBuffer, XBEE_TX_BUFFER_SIZE, "%s%s", command, XB_DELIMITER);
                    }

                    /* Force a very conspicuous integer to signal the buffer overrun condition */
                    if (bytesWritten > XBEE_TX_BUFFER_SIZE)
                    {
                        bytesWritten = XB_BUFFER_TOO_SMALL;
                    }

                    return bytesWritten;
                }

                /** Transmit Frame
                 *	Transmits a single command to the Xbee. If the device was not in command (AT) mode it will be placed 
                 *  into it. By default, the data for both transmit and receive are written to the internal tx/rx buffers.
                 *
                 *	@param[in]	command     The command to be used
                 *  @param[in]  payload     If desired, data to be attached to the frame
                 *	@return     XBStatus    XB_OK if everything is alright, error code if not
                 **/
                template<typename T = uint16_t>
                XBStatus txFrame(const char* command, T payload = 0)
                {
                    if (!command)
                    {
                        return XB_INVALID_PARAM;
                    }

                    if (!isATMode() && (goToCommandMode() != XB_OK))
                    {
                        return XB_FAILED_COMMAND_MODE;
                    }

                    int bytesWritten = frameBuilder(command, payload);
                    write(txBuffer, bytesWritten);
                    return XB_OK;
                }

                /** Transmit Frame with Result
                 *	Transmits a single command to the Xbee and returns back whatever response was given. If the device was
                 *	not in command (AT) mode it will be placed into it. By default, the data for both transmit and receive 
                 *  are written to the internal tx/rx buffers.
                 *
                 *	@param[in]	command     The command to be used
                 *  @param[in]  payload     If desired, data to be attached to the frame
                 *	@param[in]	timeout	    How long to wait (in mS) for a response before erroring out
                 *	@return     XBStatus    XB_OK if everything is alright, error code if not
                 **/
                template<typename T = uint16_t>
                XBStatus txFrameWithResult(const char* command, T payload = 0, size_t timeout_mS = XB_DEFAULT_TIMEOUT_mS)
                {
                    memset(rxBuffer, 0, XBEE_RX_BUFFER_SIZE);
                    
                    XBStatus result = txFrame(command, payload);

                    if (result == XB_OK)
                    {
                        result = readWithTimeout((uint8_t*)rxBuffer, XBEE_RX_BUFFER_SIZE, timeout_mS);
                    }

                    return result;
                }

                /** Verify Parameter
                 *  Validates that the register value stored on the XBEE for a given command matches the expected value 
                 *  
                 *  @param[in]  command     The command to be used
                 *  @param[in]  expected    The expected value read from the register
                 *  @return     XBStatus    XB_OK if everything is alright, error code if not
                 **/
                template<typename T>
                XBStatus verifyParameter(const char* command, T expected)
                {
                    if (!command)
                    {
                        return XB_INVALID_PARAM;
                    }

                    XBStatus result = txFrameWithResult(command);

                    if (result == XB_OK)
                    {
                        /* Convert the expected value into a string for easy comparison */
                        memset(txBuffer, 0, XBEE_TX_BUFFER_SIZE);
                        int bytesWritten = snprintf(txBuffer, XBEE_TX_BUFFER_SIZE, "%s", expected);

                        if (memcmp(rxBuffer, txBuffer, bytesWritten) != 0)
                        {
                            result = XB_FAILED_COMPARE;

                            #ifdef DEBUG
                            Chimera::Logging::Console.log(Chimera::Logging::Level::ERROR, "XBEE: Param compare [%s] doesn't match [%s]\r\n", rxBuffer, txBuffer);
                            #endif
                        }
                    }
                    
                    return result;
                }

                
			};

            
		}
        
    }
}

#endif /* !XBEE_PRO_SERIES_2_HPP */