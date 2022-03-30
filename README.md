# Using a Blackpill and mbed to control a PC volume control

Al Williams - Hackaday

## Build environment

The Blackpill isn't directly supported by mbed. Most people tell you to use the Nucelo target
for the F411 CPU, but that has a few quirks. 

If you use the desktop "Mbed Studio" you can follow these directions to set up for a proper 
build environment for Blackpill:

https://os.mbed.com/users/hudakz/code/BLACKPILL_Custom_Target/graph/

## Usage
The system uses a pot on one of the analog channels. Tie one end of the pot to 3.3V and the other end
to ground. Then connect the wiper to the CPU. That's it.

The CPU enumerates on the USB bus as a keyboard and sends media control keys for volume up/down and mute.
If you turn the pot up, you get volume up and down for volume down. If you get "stuck" (this can happen 
if you change the volume in some other way), just mute the audio using the button on the Blackpill (the one
furthest away from the USB connector). Adjust the pot to roughly the middle. Then unmute by pressing again.
This causes the device to reference relative to the new position. Sure, sure, it should have been
an encoder, but this is a fun example and easy to do.

