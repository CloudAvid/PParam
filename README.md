XParam Library
==============

XParam is a library based on libxml++ and contains set of classes that defines
various structures and contents (data type) of XML.

Nowadays, most applications retrieve, save, send and recieve their data in XML
format. Therefore, Our developers started to write XParam to provide a library
that eases manipulating XMLs in C++ programming language. By XParam, developers
can define a class and then print out the class in XML format or save it into a
file or load an XML from string or file to that class.
In addition, XParam introduces a novel method for designing object-oriented
programs that is called XObject. XObject defines a real world as a set of
objects and their connections. This method reduces design complexity and
increases flexibility and stability of a system.

INSTALLATION
============

The first stage is to configure XParam for your system:

	./configure

NOTE: if configure does not exist, run ./autogen script.

The next stage is to build the various binary files. Enter:

	make

Then log in as root and enter:

	make install

That's all!

See the examples directory for example code.

Use pkg-config to discover the necessary include and linker arguments. For
instance,
  pkg-config libxparam-1.0 --cflags --libs
Ideally you would use PKG_CHECK_MODULES in your configure.ac file.
