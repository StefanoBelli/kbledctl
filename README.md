# kbledctl
Keyboard LED turn on/off control 

This program born cuz of my problems with my own **CM Storm Devastator** keyboard and Linux.

~~~
 $ make
 # make install
 # kbledctl
~~~

### Usage
Just turn it on, as shown above, if needed, use *-h* to get help

### Licensing
Licensed under GNU GPLv3

### What if I want to change toggle key?
Well, pretty simple, you just have to use the -k option and specify an integer value, that value is the key you wanna use.

By default, *SCROLL LOCK* is used.

Now, you would say, how the f@#k I know the corresponding integer for the key I wanna set?

Well, just take a look at *linux/input-event-codes.h* header, which is most probably located:

~~~
/usr/include/linux/input-event-codes.h
~~~

Very easy to read, take a look. Anyway, **SCROLLLOCK** will be fine for most situations.

https://raw.githubusercontent.com/torvalds/linux/master/include/uapi/linux/input-event-codes.h

take diz

### What if I DON'T want to start it as daemon process?
You're weird. Use *-n* option.


