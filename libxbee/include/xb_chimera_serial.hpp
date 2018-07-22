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
        void write(uint8_t* data, size_t length) override;
        void read(uint8_t* data, size_t length) override;
    private:


    }
}


#endif /* !XBEE_CHIMERA_SERIAL_HPP */