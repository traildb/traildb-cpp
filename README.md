TrailDB C++ bindings
========================

![TrailDB logo](traildb_logo_512.png?raw=true)

A simple c++ wrapper for traildb C API [TrailDB](http://traildb.io/).

How to build
------------

You need to install traildb -- Follow the instructions. 



### Build test code

There is a test program that shows basic usage of the c++ API. To build it, simple run:

    $ make all

(Optionally, you can install the header files to /usr/local/include to be used
with traildb-r, e.g.)  

    $ make install  #may need sudo

Afterwards, you can run:

    $ create_tdb 

This will create a simple traildb file and loop over the trails. 
