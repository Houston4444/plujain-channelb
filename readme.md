-- README FOR PLUJAIN-CHANNELB --

ChannelB is a very simple LV2 utility plugin.<br>
This is a mono switchbox with one input and two outputs.<br>
If actived, this switchbox send its input to the second output, else the input is sent to first output.<br>
The switch is applyed with a crossfade, default crossfade is set to 50ms for clean but fast switch.<br>
You can set the fade length to 1000 seconds max (more than 16 minutes) to make a very progressive switch.<br>

There are no particular dependencies except these ones needed to build an LV2 plugin.

To build and install just type: <br>
`$ make clean && make` <br>
`$ [sudo] make install`

