### LabRedes - T1 - 2017/1
**Names**: Lasaro Dumer, Marcelo Laser

#### Compiling

The basic program will output everything in the console (stdout) and is compiled with:

	$ make

The Curses version will output the packages data in a file and keep the status updated in the console screen, compiled with:

	$ make curses


#### Uses

Always execute as root (ie, using sudo). The fisrt argument is the interface name to capture packages. A second parameter can be specified to filter the printed packages.

Therefore the basic syntax is:

	$ sudo ./sniffer.o interface [filter]

Where **filter** can be one of:

* **all:** default option if none is specified, print all packages
* **eth:** print only Ethernet
* **arp:** print only ARP
* **ipv4:** print only IPv4
* **icmp:** print only ICMP (v4)
* **tcp:** print only TCP
* **udp:** print only UDP
* **ipv6:** print only IPv6
* **icmpv6:** print only ICMP (v6)
