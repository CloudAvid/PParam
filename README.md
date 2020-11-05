# Introduction
PParam is a portable parameter (data structure) definition framework for C++.

By PParam, developers could define their parameters, and then easily convert them to XML/JSON and read/write the parameters from/to files or databases.

At now PParam supports conversion to/from XML and also have an experimental support of SQLite. 

For object-oriented programming, PParam introduces a method that called XObject. XObject defines a real world as a set of objects and their connections. This method help you to reduce complexity of design and to increase system flexibility and stability.

Any try to develop PParam is welcome.

# Installation
The first step is to get the lastest source and unzip it. run this command to configure PParam for your system:
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
Read the project [wiki](https://github.com/CloudAvid/PParam/wiki).

# PParam Roadmap
The roadmap of PParam is: 
* JSON support. 
* Attributes support in XML tags.
* Database support, PParam now has a limited support of "sqlite", so we want to have a complete DB layer, and support of different DBMS-es. By this feature, programmer could store/retrieve it's parameters in/from DB. 
* Develop more special parameters (see "sparam.hpp|cpp"). 

# Contributing Code
If you are capable of contributing code changes, we encourage you to do so. You can help us in different fields:
* Develop new features (see roadmap above)
* Testing and bug reports
* Improving Documentation
* ... 

# Bugs
 Please use github [issues](https://github.com/CloudAvid/PParam/issues) to report bugs. 
# About
Copyright 2010-2020 [CloudAvid](https://www.cloudavid.com), released under the GPL license
