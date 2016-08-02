# NRF24L01+

Single Chip 2.4GHz Transceiver

## Key Features

1. World 2.4GHz ISM band operation
2. 250kbps, 1Mbps and 2Mbps on air data rates
3. Ultra low power operation
4. 11.3mA TX at 0dBm output power
5. 13.5mA Rx at 2Mbps air data rate
6. 900nA in power down
7. 26uA in standby-1
8. On chip voltage regulator for 1.9 ~ 3.6V supply range
9. Enhanced ShockBurst
10. Automatic packet handling
11. Auto packet transcaction handling
12. 6 data pipe MultiCeiver
13. Drop-in compatible with NRF24L01
14. On-air compatible in 250kbps and 1Mbps with nRF2401A, nRF2402, nRF24E1 and nRF24E2
15. Low cost BOM
16. 60ppm 16MHz crystal
17. 5V tolerant inputs
18. Compact 20-pin 4x4mm QFN package

# Introduction

The nRF24L01+ is a single chip 2.4GHz transceiver with an embedded baseband protocol engine(Enhanced ShockBurst), suitable for ultra low power wireless applications. The nRF24L01+ is designed for operation in the world wide ISM frequency band at 2.400~2.4835GHz.

To design a radio system with the nRF24L01+, you simply need an MCU (micro controller) and a few external passive components.

You can operate and configure the nRF24L01+ through a Serial Peripheral Interface (SPI). The register map, which is accessible through the SPI, contains all configuration registers in the nRF24L01+ and is accessible in all operation modes of the chip.

The embedded baseband protocol engine (Enhanced ShockBurst) is based on packet communication and supports various modes from manual operation to advanced autonomous protocol operation. Internal FIFOs ensure a smooth data flow between the radio front end and the system's MCU. Enhanced ShockBurst reduces cost by handling all the high speed link layer operations.

The radio front end uses GFSK modulation. It has user configurable parameters like frequency channel, output power and air data rate. nRF24L01+ supports an air data rate of 250kbps, 1Mbps, 2Mbps. The high air data rate combined with two power saving modes make the nRF24L01+ very suitable for ultra low power designs.

nRF24L01+ is drop-in compatible with nRF24L01 and on-air compatible with nRF2401A, nRF2402, nRF24E1 and nRF24E2. Intermodulation and wideband blocking values in nRF24L01+ are much improved in comparison to the nRF24L01 and the addition of the internal filtering to nRF24L01+ has improved the margins for meeting RF regulatory standards.

Internal voltage regulators ensure a high Power Supply Rejection Ratio and a wide power supply range.
