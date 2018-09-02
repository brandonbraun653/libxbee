#include <libxbee/include/xb_chimera_serial.hpp>


namespace libxbee
{
	using namespace Chimera::Serial;

	XbeeChimeraSerial::XbeeChimeraSerial(uint32_t channel)
	{
		this->serial = new Chimera::Serial::SerialClass(channel);
	}

	void XbeeChimeraSerial::initialize(uint32_t baud, Modes tx_mode, Modes rx_mode)
	{
		serial->begin(baud, tx_mode, rx_mode);


		initialized = true;
	}

	void XbeeChimeraSerial::write(uint8_t* data, size_t length)
	{
		serial->write(data, length);
	}

	void XbeeChimeraSerial::read(uint8_t* data, size_t length)
	{

	}

	bool XbeeChimeraSerial::isInitialized()
	{
		return initialized;
	}
}