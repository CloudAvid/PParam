Nowadays, most applications send and receive their data in XML format. Therefore, we started to write "PParam" to  improve C++ developer productivity and performance to accessing and manipulating  XML files.

# PParam Library

PParam library is based on libxml++ . Its contains a set of classes that defines
various structures and contents (data type) of XML.

Nowadays, most applications send and receive their data in XML format. Therefore, Our developers started to write "PParam" to  improve C++ developer productivity and performance to accessing and manipulating  XML files.By PParam, developers can define a class and then print out the class in XML format or save it into a file. They also can load XML file to load class. You may see PParam as a serialize method, but with some differents.
In addition, PParam introduces a new method for designing object-oriented programs that is called XObject. XObject defines a real world as a set of objects and their connections. This method reduces design complexity and increases flexibility and stability of a system.

## PParam roadmap

The roadmap of PParam is: <i><b>Developing a general purpose parameter structure in C++ that may be used by programmers without any need of considerations about transfer or storage media of the parameters.</b></i>

# Installation

The first step is getting  the lasted source and unzipt it. run this command to configure PParam for your system:
```shell
$ ./configure
```

NOTE: if configure does not exist, run ./autogen.sh script.

The next step is building the various binary files. So, enter:
```shell
$ make
```

Then login as root and run:
```shell
$ make install
```
That's all ;-)

See the examples directory for example code.

Use pkg-config to discover the necessary include and linker arguments. For
instance,
```shell
$ pkg-config libpparam-1.0 --cflags --libs
```

# Discussion

Before any work on PParam, read PParam Tutorial from [PParam wiki](http://pdnsoft.com/en/web/pdnen/wiki/-/wiki/Main/PParam).
Also you can access to "doxygen" output of PParam comments from [PParam Doxy](http://support.pdnsoft.com/doxy/PParam).

For any discussion about PParam use it's [forum](http://pdnsoft.com/en/web/pdnen/forum/-/message_boards/category/423811).

Also you may try to report any bug/question by "github" issues.

# Collaborate

We welcome any collaboration in PParam development. It may be in different fields:
* Test and bug reports.
* Writing documents.
* Develop  new features.
* ... 

Some opportunities to develop PParam are:
* Support Of attributes for XML tags.
* Support of "json"
* Support of different databases, PParam now has a limited support of "sqlite", so we want to have a complete DB layer, and support of different DBMS-es. By this feature, programmer could store/retrieve it's parameters in/from DB.

# About PDNSoft Co.

[PDNSoft](http://www.pdnsoft.com) was established in  January 2005. The company headquarter is in Yazd ,Iran. The main goal of company is software development with open-source technology.

The most important product of us is [PVM (PDNSoft Co. Virtual Machine Management System)](http://pdnsoft.com/en/web/pdnen/pvm) which is a hypervisor based on KVM (Kernel Virtual Machine). We have been working on this product form 5 years ago. PDN is expert in building visualization servers and also in developing system softwares like of firewalls.

Our mission is to be the best high technology company through delivering  high-quality custom software We believe that the most important factor of our success is the success of our clients and provide them with products that help their business growth. We pay special attention to the quality  of the products/services  we deliver. We believe that the only way to keep our businesses successful is working hard for our customers.
