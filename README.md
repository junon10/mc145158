# MC145158 PLL Controller Arduino Library

## Description

A library for Arduino that provides functions to configure and operate the MC145158 PLL integrated circuit. This library enables control over various aspects of the PLL's operation.

## Features

- **Arduino Compatibility**: Works with most Arduino boards.
- **PLL Control Functions**: Functions to set frequency shifts, data configuration, and more.
- **Configuration**: Easy-to-use API for setting up pins and operating modes.
- **DIP Switch Integration**: Support for configuring pins via DIP switches.

## Requirements

- **Arduino IDE**: To install the library.
- **Hardware**: MC145158 PLL circuit integrated into your project.
- **Board Compatibility**: Most Arduino-compatible boards supported.

## Installation

To install the library:

1. Download the repository from GitHub:
   [https://github.com/junon10/mc145158](https://github.com/junon10/mc145158)

2. In Arduino IDE, go to Sketch > Include Library > Add ZIP File...

3. Select the downloaded zip file.

## Usage

```c++
#include <MC145158.h>

// Declare an instance of the MC145158 PLL
MC145158 pll;

// Configure the PLL pins
pll.begin(clock_pin, data_pin, dlen_pin);

// DIP Switch configuration example (Arduino Digital Pins 7 to 0)
pll.setDipSwitchPins(7, 6, 5, 4, 3, 2, 1, 0);

// Configure the operating frequency
pll.setFrequency(108000); // Frequency in KHz
// or
pll.setFrequencyFromDipSwitch();

```

## Limitations

- Warning: Code tested with logic analyzer, not yet tested in hardware!
- For detailed technical specifications, refer to the MC145158 datasheet.
- Function parameters and return values may vary based on hardware configuration.

## Author

- **Junon M.**  
  Contact: [junon10@tutamail.com](mailto:junon10@tutamail.com)

## Contributing

Contributions are welcome! Please fork the repository and send a pull request.

## Repository

- [https://github.com/junon10/mc145158](https://github.com/junon10/mc145158)

## References

- MC145158 Datasheet
- Arduino Documentation

## License

- GPLv3

## Changelog

| Version | Date        | Changes Made               |
|---------|-------------|---------------------------|
| 1.0.0.0 | 2024/06/18 | Initial commit. |
| 1.0.0.1 | 2024/06/18 | function name fix. |
| 1.0.0.2 | 2024/06/22 | bug fix. |
| 1.0.0.3 | 2025/02/07 | DipSW support. |
| 1.0.0.4 | 2025/07/23 | minor bug fix. |
| 1.0.0.5 | 2026/02/03 | fix registers N and A. |
| 1.0.0.7 | 2026/02/03 | improvements. |
| 1.0.0.8 | 2026/02/25 | Update Readme. |
| 1.0.0.9 | 2026/03/21 | Refactor. |
| 1.0.0.10 | 2026/03/22 | Msg Format Improvements. |
| 1.0.0.11 | 2026/03/24 | function name fix. |
| 1.0.0.13 | 2026/03/30 | Auto commit. |
