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

### b. how long the system is already running since last rebooted

### c. what processors it has

### d. how much RAM is available.

## 3. Copy the file nanpa, found in the home directory of theclauteruser, to your account or usescpto copythe file back home onto your private computer.

## 4. On titanic or  a  private  Linux  installation,  open  the  file nanpa using less.   The  file  contains  quite  acomprehensive list of North American phone number prefixes (first 6 digits, excluding+1), followed by thelocation this phone number prefix is attached to.  For example, for 907519, the location Anchorage AKislisted. Still insideless, find the entries for 907519,503526 and a couple of other phone numbers you knowin the country, as such your home phone, your parents’ phone, the phone of a loved one etc.

## 5. Find  out  how  many  lines  connecting  prefixes  to  locations  are  contained  in  the  filenanpa.   Which  Linuxcommand line tool do you use to count lines?

## 6. List the first 17 lines of the nanpa file on command line. Also list the last 42 lines of the file. You can use the Linux tools head and tailfor this task.

## 7. Write a short bash script findlocation that takes a 6-digit prefix in argument and displays the correspond-ing location. If the script receives no argument or the argument is not a6-digit prefix made only out of the digits 0 thru 9, the script must return an exit condition code signaling failure (e.g.  by executingexit 1).  If thescript receives a correctly formatted argument but the prefix is not found in the nanpa file, the script must returnan exit condition code signaling failure. Otherwise, the script must display the appropriate location (on stdout).The location must not be prefixed by the prefix nor followed by superfluous spaces.  This means you have to format the line found thenanpafile before displaying it. You may use grep and sed for this script.

# 3. Rewrite head and tail

# 4. File Searching
