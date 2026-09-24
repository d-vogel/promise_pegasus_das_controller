# Backplate

The backplane is connected to the controller by it's edge into a PCIe x8 connector.
![front](backplane_front.jpg)
![back](backplane_back.jpg)

Its' fonctionalities:
- route 12V and 5V to the disks
- pairs of disks can be powered on/off using `ENABLE_HDD12` and `ENABLE_HDD34` pins

- control LEDs:
    - each disk has a blue led and a (orange+blue+red) led that's 16 leds in total. These are controlled by the CPLD on the backplane. 
    - The CPLD is controlled with a special seral protocol on `LED_CLK`, `LED_DATA`, `LED_LATCH_0` and `LED_LATCH_1` pins.
    - TODO: document the protocol based on [protocol.c](../src/protocol.c)

- SATA pairs are directly routed to the connector.