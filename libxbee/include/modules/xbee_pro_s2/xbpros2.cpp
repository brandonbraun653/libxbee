#include <libxbee/include/modules/xbee_pro_s2/xbpros2.hpp>


namespace libxbee
{
	namespace modules
	{
		bool xbeeProS2::connect()
		{
			uint8_t atCmd[] = "+++";
			serial->write(atCmd, 3);
		}
	}
}
