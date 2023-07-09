# Juice Monitor

This is a driver demonstration and C3P intro project. It has no deeper purpose.

----------------------

## ESP32 support

The following three repositories must be pulled into the `components` folder. See the `downloadDeps.sh` script to see what exactly is required (or run it).

  * This project depends on [ManuvrPlatforms/ESP32](https://github.com/jspark311/ManuvrPlatforms/ESP32) to provide the fill-in for AbstractPlatform as appropriate for the ESP32.
  * This project depends on [ManuvrDrivers](https://github.com/jspark311/ManuvrDrivers) to support most of the hardware.
  * This project depends on [CppPotpourri](https://github.com/jspark311/CppPotpourri) to glue all that stuff together.


This project was written against Espressif's ESP-IDF libraries and build environment (idf4.2). So you'll need that if you want to build and flash onto hardware. Other versions will likely work, but YMMV. Blame ManuvrPlatforms.

### Building

The `activate.sh` script contains the commands I use for shell setup to use ESP-IDF...

```
source /home/ian/.espressif/python_env/idf4.2_py3.8_env/bin/activate
export PATH=$PATH:/opt/esp/tools/
```

...and the command to build and flash the firmware...

```
idf.py build size
idf.py flash monitor
```

### Usage

Using the `idf.py monitor` command will give you a nice color console with ESP-IDF's logging. But
`ParsingConsole` can be configured as desired, and used by any serial terminal program.

----------------------

## Demonstrations

### PAC195x driver

### MCP23017 driver


----------------------

#### License

Original code is Apache 2.0.

Code adapted from others' work inherits their license terms, which were preserved in the commentary where it applies.
