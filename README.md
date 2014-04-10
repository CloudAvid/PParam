PParam Library
==============

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

INSTALLATION
============

The first stage is to configure PParam for your system:

	./autogen.sh
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
  pkg-config libpparam-1.0 --cflags --libs

DISCUSSION
==========

Before any work on PParam, read PParam Tutorial from heer: http://pdnsoft.com/en/web/pdnen/wiki/-/wiki/Main/PParam

For any discussion about PParam user it's forum: http://pdnsoft.com/en/web/pdnen/forum/-/message_boards/category/423811

Also you may try to report any bug/question by "github" issues.
