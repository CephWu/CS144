# Stanford CS144

**Since 2023-12-16 cs144/minnow made private, I use cherry pick or merge to merge startercode from others.**

> Finally I found https://github.com/fei0319/CS144 which stores all the checkpoint branches.

Show all commits in the merge commit:

```txt
git log df53862^..df53862
```

Cherry pick to apply the merge commits in order (not include merge commit):

```txt
git cherry-pick start..end
git cherry-pick start~..end  # include start
```

## Check 6 Putting all together

Commands:

```txt
cd /path/to/Stanford-CS144
dd if=/dev/urandom bs=1M count=1 of=big.txt
./build/apps/endtoend server cs144.keithw.org 3000 < big.txt debug
</dev/null ./build/apps/endtoend client cs144.keithw.org 3001 > recv.txt debug

sha256sum big.txt
sha256sum recv.txt
```

1 MB = 1 \* 1024 \* 1024 Bit ~= **1000 transmissions**

Server:

```txt
DEBUG: Network interface has Ethernet address 02:00:00:3b:e7:d0 and IP address 172.16.0.1
DEBUG: Network interface has Ethernet address 02:00:00:d6:e3:ed and IP address 10.0.0.172
DEBUG: adding route 172.16.0.0/12 => (direct) on interface 0
DEBUG: adding route 10.0.0.0/8 => (direct) on interface 1
DEBUG: adding route 192.168.0.0/16 => 10.0.0.192 on interface 1
DEBUG: Network interface has Ethernet address 8a:35:5e:45:3c:09 and IP address 172.16.0.100
DEBUG: Listening for incoming connection...
     Internet->router: dst=ff:ff:ff:ff:ff:ff, src=02:00:00:c0:87:bd, type=ARP, payload: ARP: opcode=REQUEST, sender=02:00:00:c0:87:bd/10.0.0.192, target=00:00:00:00:00:00/10.0.0.172
     Router->Internet: dst=02:00:00:c0:87:bd, src=02:00:00:d6:e3:ed, type=ARP, payload: ARP: opcode=REPLY, sender=02:00:00:d6:e3:ed/10.0.0.172, target=02:00:00:c0:87:bd/10.0.0.192
     Internet->router: dst=02:00:00:d6:e3:ed, src=02:00:00:c0:87:bd, type=IPv4, payload: IPv4: IPv4, len=40, protocol=6, src=192.168.0.50, dst=172.16.0.100
     Router->host:     dst=ff:ff:ff:ff:ff:ff, src=02:00:00:3b:e7:d0, type=ARP, payload: ARP: opcode=REQUEST, sender=02:00:00:3b:e7:d0/172.16.0.1, target=00:00:00:00:00:00/172.16.0.100
     Host->router:     dst=02:00:00:3b:e7:d0, src=8a:35:5e:45:3c:09, type=ARP, payload: ARP: opcode=REPLY, sender=8a:35:5e:45:3c:09/172.16.0.100, target=02:00:00:3b:e7:d0/172.16.0.1
     Router->host:     dst=8a:35:5e:45:3c:09, src=02:00:00:3b:e7:d0, type=IPv4, payload: IPv4: IPv4, len=40, protocol=6, src=192.168.0.50, dst=172.16.0.100
     Host->router:     dst=02:00:00:3b:e7:d0, src=8a:35:5e:45:3c:09, type=IPv4, payload: IPv4: IPv4, len=40, protocol=6, src=172.16.0.100, dst=192.168.0.50
     Router->Internet: dst=02:00:00:c0:87:bd, src=02:00:00:d6:e3:ed, type=IPv4, payload: IPv4: IPv4, len=40, protocol=6, src=172.16.0.100, dst=192.168.0.50
     Internet->router: dst=02:00:00:d6:e3:ed, src=02:00:00:c0:87:bd, type=IPv4, payload: IPv4: IPv4, len=40, protocol=6, src=192.168.0.50, dst=172.16.0.100
     Router->host:     dst=8a:35:5e:45:3c:09, src=02:00:00:3b:e7:d0, type=IPv4, payload: IPv4: IPv4, len=40, protocol=6, src=192.168.0.50, dst=172.16.0.100
     Host->router:     dst=02:00:00:3b:e7:d0, src=8a:35:5e:45:3c:09, type=IPv4, payload: IPv4: IPv4, len=40, protocol=6, src=172.16.0.100, dst=192.168.0.50
     Router->Internet: dst=02:00:00:c0:87:bd, src=02:00:00:d6:e3:ed, type=IPv4, payload: IPv4: IPv4, len=40, protocol=6, src=172.16.0.100, dst=192.168.0.50
     Internet->router: dst=02:00:00:d6:e3:ed, src=02:00:00:c0:87:bd, type=IPv4, payload: IPv4: IPv4, len=40, protocol=6, src=192.168.0.50, dst=172.16.0.100
     Router->host:     dst=8a:35:5e:45:3c:09, src=02:00:00:3b:e7:d0, type=IPv4, payload: IPv4: IPv4, len=40, protocol=6, src=192.168.0.50, dst=172.16.0.100
New connection from 192.168.0.50:29802.
     Host->router:     dst=02:00:00:3b:e7:d0, src=8a:35:5e:45:3c:09, type=IPv4, payload: IPv4: IPv4, len=1040, protocol=6, src=172.16.0.100, dst=192.168.0.50
     Host->router:     dst=02:00:00:3b:e7:d0, src=8a:35:5e:45:3c:09, type=IPv4, payload: IPv4: IPv4, len=1040, protocol=6, src=172.16.0.100, dst=192.168.0.50
     Host->router:     dst=02:00:00:3b:e7:d0, src=8a:35:5e:45:3c:09, type=IPv4, payload: IPv4: IPv4, len=1040, protocol=6, src=172.16.0.100, dst=192.168.0.50
     Host->router:     dst=02:00:00:3b:e7:d0, src=8a:35:5e:45:3c:09, type=IPv4, payload: IPv4: IPv4, len=1040, protocol=6, src=172.16.0.100, dst=192.168.0.50
     Host->router:     dst=02:00:00:3b:e7:d0, src=8a:35:5e:45:3c:09, type=IPv4, payload: IPv4: IPv4, len=1040, protocol=6, src=172.16.0.100, dst=192.168.0.50
     Host->router:     dst=02:00:00:3b:e7:d0, src=8a:35:5e:45:3c:09, type=IPv4, payload: IPv4: IPv4, len=1040, protocol=6, src=172.16.0.100, dst=192.168.0.50
     Host->router:     dst=02:00:00:3b:e7:d0, src=8a:35:5e:45:3c:09, type=IPv4, payload: IPv4: IPv4, len=1040, protocol=6, src=172.16.0.100, dst=192.168.0.50
     Host->router:     dst=02:00:00:3b:e7:d0, src=8a:35:5e:45:3c:09, type=IPv4, payload: IPv4: IPv4, len=1040, protocol=6, src=172.16.0.100, dst=192.168.0.50
     Host->router:     dst=02:00:00:3b:e7:d0, src=8a:35:5e:45:3c:09, type=IPv4, payload: IPv4: IPv4, len=1040, protocol=6, src=172.16.0.100, dst=192.168.0.50
     Host->router:     dst=02:00:00:3b:e7:d0, src=8a:35:5e:45:3c:09, type=IPv4, payload: IPv4: IPv4, len=1040, protocol=6, src=172.16.0.100, dst=192.168.0.50
...
...
...
...
...
     Internet->router: dst=02:00:00:1c:7e:a7, src=02:00:00:d3:94:5b, type=IPv4, payload: IPv4: IPv4, len=40, protocol=6, src=192.168.0.50, dst=172.16.0.100
     Router->host:     dst=7e:5a:7a:cb:2a:1c, src=02:00:00:8a:fe:6c, type=IPv4, payload: IPv4: IPv4, len=40, protocol=6, src=192.168.0.50, dst=172.16.0.100
     Internet->router: dst=02:00:00:1c:7e:a7, src=02:00:00:d3:94:5b, type=IPv4, payload: IPv4: IPv4, len=40, protocol=6, src=192.168.0.50, dst=172.16.0.100
     Router->host:     dst=7e:5a:7a:cb:2a:1c, src=02:00:00:8a:fe:6c, type=IPv4, payload: IPv4: IPv4, len=40, protocol=6, src=192.168.0.50, dst=172.16.0.100
     Internet->router: dst=02:00:00:1c:7e:a7, src=02:00:00:d3:94:5b, type=IPv4, payload: IPv4: IPv4, len=40, protocol=6, src=192.168.0.50, dst=172.16.0.100
     Router->host:     dst=7e:5a:7a:cb:2a:1c, src=02:00:00:8a:fe:6c, type=IPv4, payload: IPv4: IPv4, len=40, protocol=6, src=192.168.0.50, dst=172.16.0.100
     Internet->router: dst=02:00:00:1c:7e:a7, src=02:00:00:d3:94:5b, type=IPv4, payload: IPv4: IPv4, len=40, protocol=6, src=192.168.0.50, dst=172.16.0.100
     Router->host:     dst=7e:5a:7a:cb:2a:1c, src=02:00:00:8a:fe:6c, type=IPv4, payload: IPv4: IPv4, len=40, protocol=6, src=192.168.0.50, dst=172.16.0.100
DEBUG: Outbound stream to 192.168.0.50:28460 has been fully acknowledged.
DEBUG: TCP connection finished cleanly.
done.
Exiting... done.
```

Client:

```txt
DEBUG: Network interface has Ethernet address 02:00:00:25:66:40 and IP address 192.168.0.1
DEBUG: Network interface has Ethernet address 02:00:00:4f:dd:fb and IP address 10.0.0.192
DEBUG: adding route 192.168.0.0/16 => (direct) on interface 0
DEBUG: adding route 10.0.0.0/8 => (direct) on interface 1
DEBUG: adding route 172.16.0.0/12 => 10.0.0.172 on interface 1
DEBUG: Network interface has Ethernet address 4e:94:79:9d:6f:94 and IP address 192.168.0.50
DEBUG: Connecting from 192.168.0.50:59783...
DEBUG: Connecting to 172.16.0.100:1234...
     Host->router:     dst=ff:ff:ff:ff:ff:ff, src=4e:94:79:9d:6f:94, type=ARP, payload: ARP: opcode=REQUEST, sender=4e:94:79:9d:6f:94/192.168.0.50, target=00:00:00:00:00:00/192.168.0.1
     Router->host:     dst=4e:94:79:9d:6f:94, src=02:00:00:25:66:40, type=ARP, payload: ARP: opcode=REPLY, sender=02:00:00:25:66:40/192.168.0.1, target=4e:94:79:9d:6f:94/192.168.0.50
     Host->router:     dst=02:00:00:25:66:40, src=4e:94:79:9d:6f:94, type=IPv4, payload: IPv4: IPv4, len=40, protocol=6, src=192.168.0.50, dst=172.16.0.100
     Router->Internet: dst=ff:ff:ff:ff:ff:ff, src=02:00:00:4f:dd:fb, type=ARP, payload: ARP: opcode=REQUEST, sender=02:00:00:4f:dd:fb/10.0.0.192, target=00:00:00:00:00:00/10.0.0.172
     Internet->router: dst=02:00:00:4f:dd:fb, src=02:00:00:28:cf:30, type=ARP, payload: ARP: opcode=REPLY, sender=02:00:00:28:cf:30/10.0.0.172, target=02:00:00:4f:dd:fb/10.0.0.192
     Router->Internet: dst=02:00:00:28:cf:30, src=02:00:00:4f:dd:fb, type=IPv4, payload: IPv4: IPv4, len=40, protocol=6, src=192.168.0.50, dst=172.16.0.100
     Host->router:     dst=02:00:00:25:66:40, src=4e:94:79:9d:6f:94, type=IPv4, payload: IPv4: IPv4, len=40, protocol=6, src=192.168.0.50, dst=172.16.0.100
     Router->Internet: dst=02:00:00:28:cf:30, src=02:00:00:4f:dd:fb, type=IPv4, payload: IPv4: IPv4, len=40, protocol=6, src=192.168.0.50, dst=172.16.0.100
     Internet->router: dst=02:00:00:4f:dd:fb, src=02:00:00:28:cf:30, type=IPv4, payload: IPv4: IPv4, len=40, protocol=6, src=172.16.0.100, dst=192.168.0.50
     Router->host:     dst=4e:94:79:9d:6f:94, src=02:00:00:25:66:40, type=IPv4, payload: IPv4: IPv4, len=40, protocol=6, src=172.16.0.100, dst=192.168.0.50
Successfully connected to 172.16.0.100:1234.
     Host->router:     dst=02:00:00:25:66:40, src=4e:94:79:9d:6f:94, type=IPv4, payload: IPv4: IPv4, len=40, protocol=6, src=192.168.0.50, dst=172.16.0.100
     Router->Internet: dst=02:00:00:28:cf:30, src=02:00:00:4f:dd:fb, type=IPv4, payload: IPv4: IPv4, len=40, protocol=6, src=192.168.0.50, dst=172.16.0.100
DEBUG: Outbound stream to 172.16.0.100:1234 finished (0 seqnos still in flight).
     Host->router:     dst=02:00:00:25:66:40, src=4e:94:79:9d:6f:94, type=IPv4, payload: IPv4: IPv4, len=40, protocol=6, src=192.168.0.50, dst=172.16.0.100
     Router->Internet: dst=02:00:00:28:cf:30, src=02:00:00:4f:dd:fb, type=IPv4, payload: IPv4: IPv4, len=40, protocol=6, src=192.168.0.50, dst=172.16.0.100
     Internet->router: dst=02:00:00:4f:dd:fb, src=02:00:00:28:cf:30, type=IPv4, payload: IPv4: IPv4, len=40, protocol=6, src=172.16.0.100, dst=192.168.0.50
     Router->host:     dst=4e:94:79:9d:6f:94, src=02:00:00:25:66:40, type=IPv4, payload: IPv4: IPv4, len=40, protocol=6, src=172.16.0.100, dst=192.168.0.50
     Internet->router: dst=02:00:00:4f:dd:fb, src=02:00:00:28:cf:30, type=IPv4, payload: IPv4: IPv4, len=1040, protocol=6, src=172.16.0.100, dst=192.168.0.50
     Router->host:     dst=4e:94:79:9d:6f:94, src=02:00:00:25:66:40, type=IPv4, payload: IPv4: IPv4, len=1040, protocol=6, src=172.16.0.100, dst=192.168.0.50
...
...
...
...
...
     Internet->router: dst=02:00:00:37:c4:59, src=02:00:00:f6:84:c8, type=IPv4, payload: IPv4: IPv4, len=1040, protocol=6, src=172.16.0.100, dst=192.168.0.50
     Router->host:     dst=ca:d0:d1:d2:d3:67, src=02:00:00:f4:04:21, type=IPv4, payload: IPv4: IPv4, len=1040, protocol=6, src=172.16.0.100, dst=192.168.0.50
     Host->router:     dst=02:00:00:f4:04:21, src=ca:d0:d1:d2:d3:67, type=IPv4, payload: IPv4: IPv4, len=40, protocol=6, src=192.168.0.50, dst=172.16.0.100
     Router->Internet: dst=02:00:00:f6:84:c8, src=02:00:00:37:c4:59, type=IPv4, payload: IPv4: IPv4, len=40, protocol=6, src=192.168.0.50, dst=172.16.0.100
     Internet->router: dst=02:00:00:37:c4:59, src=02:00:00:f6:84:c8, type=IPv4, payload: IPv4: IPv4, len=1040, protocol=6, src=172.16.0.100, dst=192.168.0.50
     Router->host:     dst=ca:d0:d1:d2:d3:67, src=02:00:00:f4:04:21, type=IPv4, payload: IPv4: IPv4, len=1040, protocol=6, src=172.16.0.100, dst=192.168.0.50
     Host->router:     dst=02:00:00:f4:04:21, src=ca:d0:d1:d2:d3:67, type=IPv4, payload: IPv4: IPv4, len=40, protocol=6, src=192.168.0.50, dst=172.16.0.100
     Router->Internet: dst=02:00:00:f6:84:c8, src=02:00:00:37:c4:59, type=IPv4, payload: IPv4: IPv4, len=40, protocol=6, src=192.168.0.50, dst=172.16.0.100
     Internet->router: dst=02:00:00:37:c4:59, src=02:00:00:f6:84:c8, type=IPv4, payload: IPv4: IPv4, len=1040, protocol=6, src=172.16.0.100, dst=192.168.0.50
     Router->host:     dst=ca:d0:d1:d2:d3:67, src=02:00:00:f4:04:21, type=IPv4, payload: IPv4: IPv4, len=1040, protocol=6, src=172.16.0.100, dst=192.168.0.50
     Host->router:     dst=02:00:00:f4:04:21, src=ca:d0:d1:d2:d3:67, type=IPv4, payload: IPv4: IPv4, len=40, protocol=6, src=192.168.0.50, dst=172.16.0.100
     Router->Internet: dst=02:00:00:f6:84:c8, src=02:00:00:37:c4:59, type=IPv4, payload: IPv4: IPv4, len=40, protocol=6, src=192.168.0.50, dst=172.16.0.100
     Internet->router: dst=02:00:00:37:c4:59, src=02:00:00:f6:84:c8, type=IPv4, payload: IPv4: IPv4, len=616, protocol=6, src=172.16.0.100, dst=192.168.0.50
     Router->host:     dst=ca:d0:d1:d2:d3:67, src=02:00:00:f4:04:21, type=IPv4, payload: IPv4: IPv4, len=616, protocol=6, src=172.16.0.100, dst=192.168.0.50
     Host->router:     dst=02:00:00:f4:04:21, src=ca:d0:d1:d2:d3:67, type=IPv4, payload: IPv4: IPv4, len=40, protocol=6, src=192.168.0.50, dst=172.16.0.100
     Router->Internet: dst=02:00:00:f6:84:c8, src=02:00:00:37:c4:59, type=IPv4, payload: IPv4: IPv4, len=40, protocol=6, src=192.168.0.50, dst=172.16.0.100
     Internet->router: dst=02:00:00:37:c4:59, src=02:00:00:f6:84:c8, type=IPv4, payload: IPv4: IPv4, len=1040, protocol=6, src=172.16.0.100, dst=192.168.0.50
     Router->host:     dst=ca:d0:d1:d2:d3:67, src=02:00:00:f4:04:21, type=IPv4, payload: IPv4: IPv4, len=1040, protocol=6, src=172.16.0.100, dst=192.168.0.50
DEBUG: Inbound stream from 172.16.0.100:1234 finished cleanly.
DEBUG: TCP connection finished cleanly.
     Host->router:     dst=02:00:00:f4:04:21, src=ca:d0:d1:d2:d3:67, type=IPv4, payload: IPv4: IPv4, len=40, protocol=6, src=192.168.0.50, dst=172.16.0.100
     Router->Internet: dst=02:00:00:f6:84:c8, src=02:00:00:37:c4:59, type=IPv4, payload: IPv4: IPv4, len=40, protocol=6, src=192.168.0.50, dst=172.16.0.100
DEBUG: Waiting for clean shutdown... done.
Exiting... done.
```
