# cross_build_app

- Host:

+ sudo apt-get install gcc-4.9-arm-linux-gnueabihf
+ sudo apt-get install g++-4.9-arm-linux-gnueabihf

* if package not found (on ubuntu 17 or later), try this

- on Host

+ sudo echo "deb http://cz.archive.ubuntu.com/ubuntu xenial main universe" >> /etc/apt/sources.list
+ sudo apt-get update

	and install package again. !

_ share library (mosquitto, curl, openssl, ares) in external_library

_ libssl.so.1.0.0 is symbolic link from libssl.so 
_ libcares.so.2 is symbolic link from libcares.so

* creating symbolic link for libssl.so and libcares.so library

+ cd external_library
+ ln -s /path/external_library/libssl.so libssl.so.1.0.0
+ ln -s /path/external_library/libcares.so libcares.so.2


* important! You need to edit Makefile in your project

Makefile:

#Chose compiler
+ CXX		= arm-linux-gnueabihf-g++-4.9
+ CC		= arm-linux-gnueabihf-gcc-4.9

#Library paths
+ LDFLAGS	+= -L./external_library
+ LDFLAGS	+= -L/usr/local/lib
+ LDFLAGS	+= -L/usr/include

#Library libs
+ LDLIBS	+=	-lpthread				\
		-lmosquitto				\
		-lmosquittopp				\
		-lrt					\
		-lm					\
		-lcrypto				\
		-lcurl					\
		-Xlinker -rpath=./external_library	\       /* critical */


-- save Makefile and Why does it rain ?






