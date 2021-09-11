# HHKBUSB 
Usb firmware for the Falcon

# How to build

```
make hhkbusb
```

## Note:
The built hhkbusb.bin ***MUST*** be patched with the ```patch_hhkbusb.py```, otherwise it can be updated with the bootloader.

For example:

```
python patch_hhkbusb.py hhkbusb.bin usb.bin
```

Then drag&drop the usb.bin to the bootloader.
