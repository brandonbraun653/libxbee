/**
 * @file xb_definitions.hpp
 */

 /**
 * @defgroup ATCommandOptions
 * @defgroup DiagnosticCommands
 */

/** 
 * @defgroup Coordinator 
 */

/**
 * @defgroup Router 
 */

/**
 * @defgroup EndDevice 
 */

#ifndef XBEE_DEFINITIONS_HPP
#define XBEE_DEFINITIONS_HPP

#include <stdlib.h>

namespace libxbee
{   
    #define XB_DELIMITER "\r"
    #define XB_ATxx_COMMAND_LENGTH 5      /**< The character length of all the ATxx commands possible */

	#define XB_ENTER_AT_MODE  "+++"
	#define XB_AT_MODE_RESP	  "OK\r"
	

	/**
	* @ingroup DiagnosticCommands
	* @{
	*/

	/** Firmware Version
	 *	Read firmware version of the module. The firmware version returns 4 hexadecimal values (2 bytes)
	 *	"ABCD". Digits ABC are the main release number and D is the revision number from the main release. 
	 *	"B" is a variant designator. 
	 *	
	 *	XBee and XBee-PRO ZB modules return: 0x2xxx versions\n
	 *	XBee and XBee-PRO ZNet modules return: 0x1xxx versions\n 
	 *	ZNet firmware is not compatible with ZB firmware
	 *
	 *	Parameter Range: 0-0xFFFF [read-only]\n
	 *  Parameter Default: Factory Set
	 **/
	#define XB_FIRMWARE_VER			"ATVR"

	/** Hardware Version
	 *	Read the hardware version of the module version of the module. This command can be used to
	 *	distinguish among different hardware platforms. The upper byte returns a value that is unique 
	 *	to each module type. The lower byte indicates the hardware revision. XBee ZB and XBee ZNet modules 
	 *	return the following (hexadecimal) values:\n
	 *	0x19xx - XBee module\n
	 *	0x1Axx - XBee-PRO module
	 *
	 *	Parameter Range: 0-0xFFFF [read-only]\n
	 *  Parameter Default: Factory Set
	 **/
	#define XB_HARDWARE_VER			"ATHV"

	/** @} */ /* !DiagnosticCommands */

	/**
	 * @ingroup ATCommandOptions
	 * @{
	 */

	/** Command Mode Timeout
	 *	Set/Read the period of inactivity (no valid commands received) after which the RF module
	 *	automatically exits AT Command Mode and returns to Idle Mode. 
	 *
	 *	Parameter Range: 2-0x028F [x100mS]\n
	 *	Parameter Default: 0x64 [100d]
	 **/
	#define XB_CMD_MODE_TIMEOUT		"ATCT"
    #define XB_MAX_AT_TIMEOUT_HEX   (0x028F)
    #define XB_MIN_AT_TIMEOUT_HEX   (0x0002)
    #define XB_MAX_AT_TIMEOUT_MS    ((uint16_t)65500)
    #define XB_MIN_AT_TIMEOUT_MS    ((uint16_t)200)
    #define XB_AT_TIMEOUT_MULT      ((size_t)100)

	/** Exit Command Mode
	 *  Explicitly exit the module from AT command mode
	 **/
	#define XB_CMD_MODE_EXIT		"ATCN"

	/** Guard Times
	 *	Set required period of silence before and after the Command Sequence Characters of the AT Command
	 *	Mode Sequence (GT + CC + GT). The period of silence is used to prevent inadvertent entrance into 
	 *	AT Command Mode
	 *
	 *	Parameter Range: 1-0x0CE4 [x1mS] (max of 3.3 decimal sec)\n
	 *	Parameter Default: 0x3E8  [1000d]
	 **/
	#define XB_SET_GUARD_TIME		"ATGT"
    #define XB_MAX_GUARD_TIME_MS    (3300)
    #define XB_MIN_GUARD_TIME_MS    (1)

	/** Command Sequence Character
	 *	Set/Read the ASCII character value to be used between Guard Times of the AT Command Mode Sequence 
	 *	(GT + CC + GT). The AT Command Mode Sequence enters the RF module into AT Command Mode. The CC 
	 *	command is only supported when using AT firmware: 20xx (AT coordinator), 22xx (AT router), 28xx (AT
	 *	end device)
	 *
	 *	Parameter Range: 0-0xFF\n
	 *	Parameter Default: 0x2B ('+' ASCII)
	 **/
	#define XB_SET_CMD_SEQ_CHAR		"ATCC"

	/** @} */ /* !ATCommandOptions */


	enum XBStatus : int
	{
        XB_BUFFER_TOO_SMALL = -1234,
		XB_OK,
        XB_BAD_RESULT,
		XB_UNKNOWN_ERROR,
		XB_TIMEOUT,
		XB_BUFFER_OVERRUN,
		XB_NO_RESPONSE,
		XB_BAD_RESPONSE,
		XB_INVALID_ADDRESS,
        XB_INVALID_PARAM,
		XB_NOT_FOUND,
        XB_FAILED_COMMAND_MODE,
        XB_FAILED_COMPARE,

		XB_NUMBER_OF_STATUS_KEYS
	};

    enum XBCommandCategory : uint8_t
    {
        GENERIC,
        EXTENDED,

    };

	struct Config
	{
		size_t commandModeTimeout;		        /**< How many milliseconds the Xbee should stay in AT mode before timing out */
        size_t guardTime;                       /**< Required number of milliseconds of silence before and after the command sequence to enter AT mode */
	};


	

	struct Version
	{
        uint16_t firmwareVersion;
		uint16_t hardwareVersion;
	};



    template<typename Payload>
    class CommandPacket
    {
    public:
        XBCommandCategory cmdType;

        char ATCommand[XB_ATxx_COMMAND_LENGTH];     /**< The character sequence for the given command */
        Payload payload;
        size_t payloadSize;
        size_t timeout;                         /**< */
    private:

    };

    
}

#endif /* !XBEE_DEFINITIONS_HPP */