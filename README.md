# Automatic doorman for the mosque of Gland

Automatique door opener for the mosque of Gland (CCMN) by using:
- An Arduino Uno
- The phone SIM hat [SIM7600CE-T_4G](https://wiki.dfrobot.com/SIM7600CE-T_4G(LTE)_Shield_V1.0_SKU_TEL0124) for Arduino. Purchased on [Mouser](https://eu.mouser.com/ProductDetail/DFRobot/TEL0124?qs=17u8i%2FzlE88MEbXRJuYFsA%3D%3D)
- The SIM card used by the genuine mosque's phone

The device waits indefinitely for phone call and does this series of actions every time:

1. Answer to phone call
2. Wait a bit
3. Send DTMF code **#61**
4. Wait a bit
5. Hang up phone call
6. Listen and wait again for new phone call

# Code setup

The easiest way to use this project is by using the official **Arduino IDE**.

However, you can follow theses instructions if you prefer to use VS Code.

## OS Dependencies

On Arch Linux, install these packages: `extra/arduino-avr-core` and `extra/arduino`

## VS Code extension

Open project with vscode

```sh
code portier/
```

install the [Arduino extension](https://marketplace.visualstudio.com/items?itemName=vsciot-vscode.vscode-arduino).

Initialize the extension for **Arduino Uno** with vscode command `Arduino: initialize`
