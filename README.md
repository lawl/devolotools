Script(s?) to mess with devolo powerline adapters
=================================================

* Contains a script to decrypt the config file you can download from the web interface.
They append a checksum at the end I haven't reversed yet~~, so it's not possible to edit the config files yet. Just to view.~~ See below.

* In `dLAN500-ROOT/` you can find instructions to root the dLAN 500 WiFi (as of 3.1.3). I was not able to test different devices.

Signing arbitrary content as a valid devolo config file
------------------------------------------------------

The webinterface dumps the config file to /tmp via /usr/sbin/chunk first, then reads it and encrypts/signs it as a devolo config you can import again. So now we simply need to race webinterface and replace the config quickly enough.
This line will do:

    while [ ! -f /tmp/config ];do echo polling...;done;echo hax > /tmp/config

Now download the config in the webinterface, and you'll have a valid and signed config file containing "hax".

Next step: Check if we can dump a v3 config (we can). Modify it to start telnet. Use the v2 firmware to sign it. Import it back in v3.
