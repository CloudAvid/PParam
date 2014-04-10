# PParam Library

PParam is a library based on libxml++ and contains set of classes that defines
various structures and contents (data type) of XML.

Nowadays, most applications retrieve, save, send and recieve their data in XML
format. Therefore, Our developers started to write PParam to provide a library
that eases manipulating XMLs in C++ programming language. By PParam, developers
can define a class and then print out the class in XML format or save it into a
file or load an XML from string or file to that class. You may see PParam 
as a serialize method, but with some differ.
In addition, PParam introduces a novel method for designing object-oriented
programs that is called XObject. XObject defines a real world as a set of
objects and their connections. This method reduces design complexity and
increases flexibility and stability of a system.

## PParam Target

The Target of PParam is: <i><b>Developing a general purpose parameter structure in C++ that may be used by programmers without any need of considerations about transfer or storage media of the parameters.</b></i>

# Installation

The first stage is to configure PParam for your system:
```shell
$ ./configure
```

NOTE: if configure does not exist, run ./autogen.sh script.

The next stage is to build the various binary files. Enter:
```shell
$ make
```

Then log in as root and enter:
```shell
$ make install
```
That's all!

See the examples directory for example code.

Use pkg-config to discover the necessary include and linker arguments. For
instance,
```shell
$ pkg-config libpparam-1.0 --cflags --libs
```

# Discussion

Before any work on PParam, read PParam Tutorial from [PParam wiki](http://pdnsoft.com/en/web/pdnen/wiki/-/wiki/Main/PParam).

For any discussion about PParam use it's [forum](http://pdnsoft.com/en/web/pdnen/forum/-/message_boards/category/423811).

Also you may try to report any bug/question by "github" issues.

# Collaborate

We welcome any collaboration in PParam development. It may be in different fields:
* Test and bug reports.
* Writing documents.
* Develop new features.
* ... 

Some opportunities to develop PParam are:
* Support Of attributes for XML tags.
* Support of "json"
* Support of different databases, PParam now has a limited support of "sqlite", so we want to have a complete DB layer, and support of different DBMS-es. By this feature, programmer could store/retrieve it's parameters in/from DB.

