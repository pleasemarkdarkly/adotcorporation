# default.dcl: configuration for basic board tests
# danc@iobjects.com 08/09/01
# (c) Interactive Objects

name tests
type system

tests allocate.cpp alloclimit.cpp dumpflash.cpp

dist tests/allocate.cpp tests/dumpflash.cpp
