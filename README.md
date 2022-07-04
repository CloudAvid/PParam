# PParam module

- [PParam module](#pparam-module)
  - [Introduction](#introduction)
  - [Installation](#installation)
  - [Documentation](#documentation)
  - [Contribution](#contribution)
  - [About](#about)

## Introduction

**PParam** - aka. **P**ortable **Param**eter - supplies a data structure definition framework for `C++` programming language.

By `PParam`, developers could define their parameters, and provided with easy and subtle conversion to `XML/JSON`.  
In which framework ,a handler is given for managing these parameters from/to files or databases.

One of the key feature of `PParam` framework is achieved through `XObject` implementation.

- **XObject** is designed to deliver `OOP` (**O**bject-**o**riented **p**rogramming) specification to this framework. By which ,reduction of design complexity alongside of system flexibility and stability expansion would be obtained.

## Installation

Installation of `PParam` is based on `Autoconf` and `Autotools` build system.
> Therefore, make sure they are installed properly before you proceed.

1. First step is to clone the project in desired directory:

    ```bash
    # Your favorite directory
    $ git clone https://github.com/CloudAvid/PParam.git
    ```

2. Then for `Makefiles` creation issue these commands at cloned directory.

    ```bash
    # Creates Configuration files.
    $ ./autogen.sh

    # Creates Makefiles for building project.
    $ ./configure
    ```

    > You'd be prompted with **unmet requirements** which **must be** fulfilled before proceeding.

3. After successful `Makefile` creation; let us proceed to `PParam` compilation and installation.
      - At this point (specificity *installation* part) you need `superuser` privilege.

    ```bash
    # Compile the project.
    $ make
    # Let's install it.
    $ make install
    ```

> Use pkg-config to discover the necessary include and linker arguments. Issue this:

```bash
# Displays pparam necessary liker and compile flags.
pkg-config libpparam-1.0 --cflags –libs
```

## Documentation

Read the project [wiki](https://github.com/CloudAvid/PParam/wiki).

## Contribution

If you are capable of contributing code changes, we encourage you to do so. You can help us in different fields:

- Bug reports
- Improving documentation
- Providing test suit
- ...

For better set of needed contribution please check [issues](https://github.com/CloudAvid/PParam/issues) or even report new ones :smile.

## About

This project brought you by [CloudAvid](https://www.cloudavid.com) developer team.
