# Introduction
PParam is an open source library based on libxml++ . Its contains a set of classes that defines various structures and contents (data type) of XML.
Today, most applications send and receive their data in XML format. Therefore, Our developers started to write "PParam" to improve C++ developer productivity and performance to accessing and manipulating XML files.
By PParam, developers can define a class and then print out the class in XML format and save it into a file. They also can load XML data directly into class!.
You may see PParam as a serialize method, but with some different. In addition, PParam introduces a new method for designing object-oriented programs that is called XObject. XObject defines a real world as a set of objects and their connections. This method reduces design complexity and increases flexibility and stability of a system.
# Installation
The first step is getting the lasted source and unzipt it. run this command to configure PParam for your system:
```shell
$ ./configure
```
NOTE: if the configure file does not exist, run ./autogen.sh script.
The next step is building the various binary files. So, enter:
```shell
$ make
```
Then login as root and run:
```shell
$ make install
```
That's all ;-)

Use pkg-config to discover the necessary include and linker arguments. For instance,
```shell
$ pkg-config libpparam-1.0 --cflags –libs
```
# Documentation
Our wiki contains several [tutorials](http://pdnsoft.com/en/web/pdnen/wiki/-/wiki/Main/PParam) on how to use PParam.
if you are looking for a more in-depth view of how PParam works, we recommend you to look at "sparam.hpp|cpp" files. We also put the "doxygen" output of PParam comments [here](http://support.pdnsoft.com/doxy/PParam)
If you have questions regarding the use of PParam  feel free to register and ask your question on [forum](http://pdnsoft.com/en/web/pdnen/forum/-/message_boards/category/423811). 
# PParam Roadmap
The roadmap of PParam is: 
* Developing a general purpose parameter structure in C++ that can be used by programmers without any worries about  transferring or storage media.
* Attributes support in XML tags.
* "json" support. 
* Database support, PParam now has a limited support of "sqlite", so we want to have a complete DB layer, and support of different DBMS-es. By this feature, programmer could store/retrieve it's parameters in/from DB. 
* Develop more special parameters (see "sparam.hpp|cpp"). 

# Contributing Code
If you are capable of contributing code changes, we encourage you to do so. You can help us in different fields:
Testing and bug reports
Improving Documentation
Develop new features (see opportunities below)
... 

# Bugs
 Please use github [issues](https://github.com/pdnsoft/PParam/issues) to report bugs. 
# About
Copyright 2010-2014 [PDNSoft Co.](http://www.pdnsoft.com), released under the GPL license
