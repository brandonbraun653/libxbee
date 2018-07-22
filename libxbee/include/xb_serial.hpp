#ifndef XB_SERIAL_HPP
#define XB_SERIAL_HPP

#include <stdlib.h>
#include <stdint.h>

namespace libxbee
{
	class XBEESerial
	{
	public:
		virtual void write(uint8_t* data, size_t length) = 0;
		virtual void read(uint8_t* data, size_t length) = 0;

	private:

	};
}



#endif /* !XB_SERIAL_HPP */