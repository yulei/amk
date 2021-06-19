# AMK 
Keyboard firmware for NRF52 and STM32. 
[TMK](https://github.com/tmk/tmk_keyboard.git) was used for key processing.
[tinyusb](https://github.com/hathach/tinyusb.git) was used for usb protocol handling.
NRF5 SDK and STM32 HAL library were used for driving peripherals of the MCU.

# How to build

```
git clone https://github.com/yulei/amk.git
cd amk
git submodule init
git submodule update
make onekey
```

Current available keyboards can be listed with:

```
make
```

## Note:
If want to compile keyboards which were made by nrf52 (like the hhkbble), the [NRF5 SDK](https://www.nordicsemi.com/Software-and-Tools/Software/nRF5-SDK) must be downloaded and unzipped to the ```vendor/nordic``` directory. The current version used by amk was 17.0.2.
