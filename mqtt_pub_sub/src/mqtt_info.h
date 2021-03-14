///////////////////////////////////////////////////////////
//////////////// Creating global variables ////////////////
///////////////////////////////////////////////////////////

// Pins on Arduino boards that send the signals to turn off the pumps/relays
const int switchPin1 = 14;
const int switchPin2 = 27;

// Name of water pumps so I can identify which of these are working 
char* pumpName1 = "pump_1";
char* pumpName2 = "pump_2";

// Topics that switch on the water pumps
char const* switchTopic1 = "esp32/sub/data/house/switch1/";
char const* switchTopic2 = "esp32/sub/data/house/switch2/";

// Where the confirmation messages are sent when the pump is turned on
char* switchConfirmTopic1 = "esp32/pub/data/house/switchConfirm1/";
char* switchConfirmTopic2 = "esp32/pub/data/house/switchConfirm2/";