
/* Local includes */
#include "JuiceMonitor.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include <time.h>
#include <sys/time.h>

#ifdef __cplusplus
}
#endif


/*******************************************************************************
* Globals
*******************************************************************************/
const char* PROGRAM_NAME       = "JuiceMonitor";
const char* console_prompt_str = "JuiceMonitor # ";

ESP32StdIO console_uart;
ParsingConsole console(128);


/* This bus handles UI and the baro sensor. */
const I2CAdapterOptions i2c0_opts(
  0,   // Device number. This is typically a platform identifier.
  SDA0_PIN,  // (sda)
  SCL0_PIN,  // (scl)
  0,   // No pullups. No point, since the ESP32's pull-ups are insufficient for i2c.
  200000  // 200kHz
);

I2CAdapter i2c0(&i2c0_opts, 4, 24);


/* Power monitoring sensor. */
PAC195xConfig pac1953_conf(
  (PAC195X_CHAN_MASK_0 | PAC195X_CHAN_MASK_1 | PAC195X_CHAN_MASK_2),
  PAC195xMode::SPS_64
);

PAC195x pwr_sensor(
  &pac1953_conf,
  PAC195X_I2C_ADDR,     // Bus address
  PAC195X_ALERT1_PIN,   // No ALERT1 pin
  PAC195X_ALERT2_PIN,   // No ALERT2 pin
  PAC195X_PWRDOWN_PIN   // No PWR_DWN pin
);


/* Simple demo of a GPIO explander driver. */
MCP23017 offboard_gpio(MCP23017_I2C_ADDR, MCP23017_RESET_PIN, MCP23017_IRQ_PIN);

/* Profiling data */
uint32_t boot_time         = 0;      // millis() at boot.
uint32_t config_time       = 0;      // millis() at end of setup().
StopWatch stopwatch_c3p_loop;


/*******************************************************************************
* Process functions called from the main service loop.
*******************************************************************************/


/*******************************************************************************
* SX1503 callbacks
*******************************************************************************/

void mcp23017_callback_fxn(uint8_t pin, uint8_t level) {
  switch (pin) {
    case TEST_PIN_0_PIN:  break;
    case TEST_PIN_1_PIN:  break;
    case TEST_PIN_2_PIN:  break;
    case TEST_PIN_3_PIN:  break;
    case TEST_PIN_4_PIN:  break;
    case TEST_PIN_5_PIN:  break;
    case TEST_PIN_6_PIN:  break;
    case TEST_PIN_7_PIN:  break;
    case TEST_PIN_8_PIN:  break;
    case TEST_PIN_9_PIN:  break;
    case TEST_PIN_A_PIN:  break;
    case TEST_PIN_B_PIN:  break;
    case TEST_PIN_C_PIN:  break;
    case TEST_PIN_D_PIN:  break;
    case TEST_PIN_E_PIN:  break;
    case TEST_PIN_F_PIN:  break;
    default:
      break;
  }
}



/*******************************************************************************
* Console callbacks
*******************************************************************************/

/* Direct console shunt */
int callback_help(StringBuilder* txt_ret, StringBuilder* args) {
  return console.console_handler_help(txt_ret, args);
}

/* Direct console shunt */
int callback_console_tools(StringBuilder* txt_ret, StringBuilder* args) {
  return console.console_handler_conf(txt_ret, args);
}


int callback_mcp23017_test(StringBuilder* text_return, StringBuilder* args) {
  int ret = -1;
  char* cmd = args->position_trimmed(0);
  // We interdict if the command is something specific to this application.
  if (0 == StringBuilder::strcasecmp(cmd, "reconf")) {
  }
  else ret = offboard_gpio.console_handler(text_return, args);

  return ret;
}


int callback_i2c_tools(StringBuilder* text_return, StringBuilder* args) {
  int ret = -1;
  if (0 < args->count()) {
    int bus_id = args->position_as_int(0);
    args->drop_position(0);
    switch (bus_id) {
      case 0:   ret = i2c0.console_handler(text_return, args);  break;
      default:
        text_return->concatf("Unsupported bus: %d\n", bus_id);
        break;
    }
  }
  return ret;
}




/*******************************************************************************
* Main function and threads                                                    *
*******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/**
* This is the Manuvr thread that runs constatntly, and would be the main loop
*   of a single-threaded program.
*/
void c3p_task(void* pvParameter) {
  while (1) {
    bool should_sleep = true;
    stopwatch_c3p_loop.markStart();

    pwr_sensor.poll();
    offboard_gpio.poll();

    if (pwr_sensor.chan_1.fresh()) {
      //voltage_filter.feedFilter(pwr_sensor.chan_1.voltage());
      //should_sleep = false;
    }

    if (0 < console_uart.poll()) {
      should_sleep = false;
    }

    stopwatch_c3p_loop.markStop();
    if (should_sleep) {
      platform.yieldThread();
    }
  }
}



/*******************************************************************************
* This will be the entry-point for our code from ESP-IDF's boilerplate. If we
*   don't trust or want the sdkconfig choice about stack depth of this thread,
*   launch any service threads we want
* If you DO trust that this thread will serve your needs, it is not necessary to
*   spawn a new one for the sake of the C3P components.
*
* Here, we opt to treat our entry thread as if it were the Setup() function in
*   an an Arduino-esque sketch. We do our setup of global resources, spawn a
*   service thread, and let this thread die.
*******************************************************************************/
void app_main() {
  /*
  * The platform object is created as a global in ManuvrPlatforms, but takes no
  *   action upon construction. The first thing that should be done is to call
  *   platform_init() to setup the defaults of the platform.
  */
  platform_init();
  boot_time = millis();

  // Top-level pin responsibilities. Normally, hardware drivers handle their
  //   own pins and interrupt service functions. But usually a project has a
  //   few pins that need to be handled at the top level.
  // This would be a good time to do that.
  pinMode(LED_PIN, GPIOMode::ANALOG_OUT);   // We PWM the LED pin.
  analogWrite(LED_PIN, 0.0f);               // And start with it off.

  /* Start the console UART and attach it to the console. */
  console_uart.readCallback(&console);    // Attach the UART to console...
  console.setOutputTarget(&console_uart); // ...and console to UART.
  console.setTXTerminator(LineTerm::CRLF); // Best setting for "idf.py monitor"
  console.setRXTerminator(LineTerm::LF);   // Best setting for "idf.py monitor"
  console.setPromptString(console_prompt_str);
  console.emitPrompt(true);
  console.localEcho(true);
  console.printHelpOnFail(true);

  console.defineCommand("help",        '?',  "Prints help to console.", "[<specific command>]", 0, callback_help);
  console.defineCommand("console",     '\0', "Console conf.", "[echo|prompt|force|rxterm|txterm]", 0, callback_console_tools);
  console.defineCommand("mcp",         'm',  "MCP23017 test", "", 0, callback_mcp23017_test);
  console.defineCommand("i2c",         'I',  "I2C tools", "i2c <bus> <action> [addr]", 1, callback_i2c_tools);
  platform.configureConsole(&console);

  console.init();

  StringBuilder ptc(PROGRAM_NAME);
  ptc.concat(" Build date " __DATE__ " " __TIME__ "\n");

  i2c0.init();

  // Assign i2c0 to devices attached to it.
  pwr_sensor.setAdapter(&i2c0);
  offboard_gpio.setAdapter(&i2c0);

  offboard_gpio.attachInterrupt(TEST_PIN_4_PIN, mcp23017_callback_fxn, IRQCondition::CHANGE);
  offboard_gpio.attachInterrupt(TEST_PIN_D_PIN, mcp23017_callback_fxn, IRQCondition::RISING);
  offboard_gpio.attachInterrupt(TEST_PIN_E_PIN, mcp23017_callback_fxn, IRQCondition::FALLING);

  // Write our boot log to the UART.
  console.printToLog(&ptc);

  // Spawn worker thread for CppPotpourri's use.
  xTaskCreate(c3p_task, "_c3p_poll", 16000, NULL, (tskIDLE_PRIORITY), NULL);

  // Note the time it took to do setup, and allow THIS thread to gracefully terminate.
  config_time = millis();
}

#ifdef __cplusplus
}
#endif
