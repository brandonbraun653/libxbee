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

namespace libxbee
{
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
	#define XB_FIRMWARE_VER			"VR"

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
	#define XB_HARDWARE_VER			"HV"

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
	#define XB_CMD_MODE_TIMEOUT		"CT"

	/** Exit Command Mode
	 *  Explicitly exit the module from AT command mode
	 **/
	#define XB_CMD_MODE_EXIT		"CN"

	/** Guard Times
	 *	Set required period of silence before and after the Command Sequence Characters of the AT Command
	 *	Mode Sequence (GT + CC + GT). The period of silence is used to prevent inadvertent entrance into 
	 *	AT Command Mode
	 *
	 *	Parameter Range: 1-0x0CE4 [x1mS] (max of 3.3 decimal sec)\n
	 *	Parameter Default: 0x3E8  [1000d]
	 **/
	#define XB_SET_GUARD_TIME		"GT"

	/** Command Sequence Character
	 *	Set/Read the ASCII character value to be used between Guard Times of the AT Command Mode Sequence 
	 *	(GT + CC + GT). The AT Command Mode Sequence enters the RF module into AT Command Mode. The CC 
	 *	command is only supported when using AT firmware: 20xx (AT coordinator), 22xx (AT router), 28xx (AT
	 *	end device)
	 *
	 *	Parameter Range: 0-0xFF\n
	 *	Parameter Default: 0x2B ('+' ASCII)
	 **/
	#define XB_SET_CMD_SEQ_CHAR		"CC"

	/** @} */ /* !ATCommandOptions */



	enum XBStatus : int
	{
		XB_OK,
		XB_UNKNOWN_ERROR,
		XB_TIMEOUT,
		XB_BUFFER_OVERRUN,
		XB_NO_RESPONSE,
		XB_BAD_RESPONSE,
		XB_INVALID_ADDRESS,
		XB_NOT_FOUND,

		XB_NUMBER_OF_STATUS_KEYS
	};

	struct Config
	{
		uint32_t atModeTimeout;		/**< How many milliseconds the Xbee should stay in AT mode before timing out */
	};

	struct Version
	{
		uint16_t firmwareVersion;
		uint16_t hardwareVersion;
	};



}

#endif /* !XBEE_DEFINITIONS_HPP */