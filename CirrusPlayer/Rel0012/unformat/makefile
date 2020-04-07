all: unformat.exe

unformat.exe: unformat.c bootcode.h
        cl -nologo unformat.c

bootcode.h: makeboot.exe image\bootcode.rom
        makeboot

makeboot.exe: makeboot.c
        cl -nologo makeboot.c

clean:
        @if exist *~ erase *~
        @if exist unformat.obj erase unformat.obj
        @if exist unformat.exe erase unformat.exe
        @if exist makeboot.obj erase makeboot.obj
        @if exist makeboot.exe erase makeboot.exe
        @if exist bootcode.h erase bootcode.h
