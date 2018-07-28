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

				/** Discovery of the Xbee
				 *	Attempts to connect to the Xbee and reconfigure it to desired baud rate. This is under the assumption
				 *	that the device's serial port is configured as 8N1. If connection cannot be established at the given baud
				 *	rate, the software will search through all available standard baud rates for the device, error-ing out if 
				 *	it cannot be found.
				 *
				 *	@param[in]	baud	Desired baud rate to use for communication
				 *	@return XB_OK if device was found, XB_NOT_FOUND if not
				 **/
				XBStatus discover(Chimera::Serial::BaudRate baud);

				/** Initializes an Xbee after discovery has been performed
				 *	Attempts to reprogram Xbee settings according to a configuration struct. All settings are verified by
				 *	reading back of the data after writing.
				 *
				 *	@param[in] config	Configuration struct for how the Xbee should be set up
				 *	@return XB_OK if everything is alright, error code from XBStatus if not
				 **/
				XBStatus initialize(const Config& config);

				/** Pings a device to see if it is still alive
				 *	@return true if alive, false if dead
				 **/
				bool ping();


				Version getVersion();
				
				/** Transmit Command with Result
				 *	Transmits a single command to the Xbee and returns back whatever response was given. If the device was 
				 *	not in command (AT) mode during the first attempt, a second attempt will be made after putting the Xbee
				 *	into command mode. The specified timeout will apply to both operations, yielding a max delay of 2x. If
				 *	neither attempt works, an error code will be returned.
				 *
				 *	@param[in]	dataIn	Address of array to transmit from
				 *	@param[in]	lenIn	Size of dataIn array OR how many bytes to transmit
				 *	@param[out]	dataOut	Address of array to store return data
				 *	@param[in]	lenOut	Size of dataOut array OR how many bytes to receive
				 *	@param[in]	timeout	How long to wait (in mS) for a response before error-ing out 
				 *	@return XB_OK if everything is alright, error code from XBStatus if not
				 **/
				XBStatus txCommandWithResult(uint8_t* dataIn, size_t lenIn, uint8_t* dataOut, size_t lenOut, size_t timeout);

				XBEEProS2(int serialChannel, Chimera::GPIO::Port rstPort, uint8_t rstPin);
				~XBEEProS2() = default;

			private:
				Chimera::Serial::SerialClass* serial;
				Chimera::GPIO::GPIOClass* reset;

				bool device_attached = false;
				
				XBStatus goToCommandMode();

				void sendCommand(const char* data, size_t length);

				XBStatus readResultWithTimeout(uint8_t* data, size_t length, size_t timeout_mS);

				void reconfigureBaudrate(Chimera::Serial::BaudRate baud);
			};
		}
        
    }
}

#endif /* !XBEE_PRO_SERIES_2_HPP */