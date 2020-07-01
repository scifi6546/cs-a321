# 1. Linux ssh login



# 2. Getting to know the bash shell

## 1. Using ssh, log into the remote system titaniconquirin.is-a-geek.org
I logged into the remote system.
```
Linux titanic 4.19.118-v7l+ #1311 SMP Mon Apr 27 14:26:42 BST 2020 armv7l

The programs included with the Debian GNU/Linux system are free software;
the exact distribution terms for each program are described in the
individual files in /usr/share/doc/*/copyright.

Debian GNU/Linux comes with ABSOLUTELY NO WARRANTY, to the extent
permitted by applicable law.
Last login: Mon Jun 29 15:57:20 2020 from 206.174.94.250
naalexeev@titanic:~ $ 
```

## 2. On that system find out

### a. how many users are logged in concurrently with you

I ran users.
```
naalexeev@titanic:~ $ users
amwilliams24 enfasoformoso enfasoformoso jfrippe mddimatulac naalexeev
```
### b. how long the system is already running since last rebooted

I used uptime.
```
naalexeev@titanic:~ $ uptime
 11:18:10 up 5 days, 22:46,  6 users,  load average: 0.08, 0.06, 0.01
```
### c. what processors it has

I read the file /proc/cpuinfo. The server has a ARMv7 processor
```
naalexeev@titanic:~ $ cat /proc/cpuinfo 
processor	: 0
model name	: ARMv7 Processor rev 3 (v7l)
BogoMIPS	: 108.00
Features	: half thumb fastmult vfp edsp neon vfpv3 tls vfpv4 idiva idivt vfpd32 lpae evtstrm crc32 
CPU implementer	: 0x41
CPU architecture: 7
CPU variant	: 0x0
CPU part	: 0xd08
CPU revision	: 3

processor	: 1
model name	: ARMv7 Processor rev 3 (v7l)
BogoMIPS	: 108.00
Features	: half thumb fastmult vfp edsp neon vfpv3 tls vfpv4 idiva idivt vfpd32 lpae evtstrm crc32 
CPU implementer	: 0x41
CPU architecture: 7
CPU variant	: 0x0
CPU part	: 0xd08
CPU revision	: 3

processor	: 2
model name	: ARMv7 Processor rev 3 (v7l)
BogoMIPS	: 108.00
Features	: half thumb fastmult vfp edsp neon vfpv3 tls vfpv4 idiva idivt vfpd32 lpae evtstrm crc32 
CPU implementer	: 0x41
CPU architecture: 7
CPU variant	: 0x0
CPU part	: 0xd08
CPU revision	: 3

processor	: 3
model name	: ARMv7 Processor rev 3 (v7l)
BogoMIPS	: 108.00
Features	: half thumb fastmult vfp edsp neon vfpv3 tls vfpv4 idiva idivt vfpd32 lpae evtstrm crc32 
CPU implementer	: 0x41
CPU architecture: 7
CPU variant	: 0x0
CPU part	: 0xd08
CPU revision	: 3

Hardware	: BCM2835
Revision	: c03111
Serial		: 10000000900d629c
Model		: Raspberry Pi 4 Model B Rev 1.1
```


### d. how much RAM is available.

I ran the program free. The program has 2.5 Gibibyte free.
```
naalexeev@titanic:~ $ free -h
              total        used        free      shared  buff/cache   available
Mem:          3.8Gi       299Mi       2.5Gi       128Mi       1.0Gi       3.3Gi
Swap:          99Mi          0B        99Mi
```
## 3. Copy the file nanpa, found in the home directory of the clauteruser, to your account or use scp to copythe file back home onto your private computer.

I used cp to copy the file.
```
naalexeev@titanic:~ $ cp /home/clauter/nanpa ~/napa
```

## 4. On titanic or  a  private  Linux  installation,  open  the  file nanpa using less.   The  file  contains  quite  acomprehensive list of North American phone number prefixes (first 6 digits, excluding+1), followed by thelocation this phone number prefix is attached to.  For example, for 907519, the location Anchorage AKislisted. Still insideless, find the entries for 907519,503526 and a couple of other phone numbers you knowin the country, as such your home phone, your parents’ phone, the phone of a loved one etc.

## 5. Find  out  how  many  lines  connecting  prefixes  to  locations  are  contained  in  the  filenanpa.   Which  Linuxcommand line tool do you use to count lines?

## 6. List the first 17 lines of the nanpa file on command line. Also list the last 42 lines of the file. You can use the Linux tools head and tailfor this task.

## 7. Write a short bash script findlocation that takes a 6-digit prefix in argument and displays the correspond-ing location. If the script receives no argument or the argument is not a6-digit prefix made only out of the digits 0 thru 9, the script must return an exit condition code signaling failure (e.g.  by executingexit 1).  If thescript receives a correctly formatted argument but the prefix is not found in the nanpa file, the script must returnan exit condition code signaling failure. Otherwise, the script must display the appropriate location (on stdout).The location must not be prefixed by the prefix nor followed by superfluous spaces.  This means you have to format the line found thenanpafile before displaying it. You may use grep and sed for this script.

# 3. Rewrite head and tail

# 4. File Searching
