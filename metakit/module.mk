MODULE := metakit
SRC_PREFIX := $(MODULE)/src
HDR_PREFIX := $(MODULE)/include

HDR += $(HDR_PREFIX)/column.h \
       $(HDR_PREFIX)/column.inl \
       $(HDR_PREFIX)/config.h \
       $(HDR_PREFIX)/custom.h \
       $(HDR_PREFIX)/derived.h \
       $(HDR_PREFIX)/field.h \
       $(HDR_PREFIX)/field.inl \
       $(HDR_PREFIX)/format.h \
       $(HDR_PREFIX)/gnuc.h \
       $(HDR_PREFIX)/handler.h \
       $(HDR_PREFIX)/handler.inl \
       $(HDR_PREFIX)/header.h \
       $(HDR_PREFIX)/mfc.h \
       $(HDR_PREFIX)/mk4.h \
       $(HDR_PREFIX)/mk4.inl \
       $(HDR_PREFIX)/mk4io.h \
       $(HDR_PREFIX)/mk4str.h \
       $(HDR_PREFIX)/mk4str.inl \
       $(HDR_PREFIX)/msvc.h \
       $(HDR_PREFIX)/persist.h \
       $(HDR_PREFIX)/std.h \
       $(HDR_PREFIX)/store.h \
       $(HDR_PREFIX)/store.inl \
       $(HDR_PREFIX)/univ.h \
       $(HDR_PREFIX)/univ.inl \
       $(HDR_PREFIX)/win.h

SRC += $(SRC_PREFIX)/column.cpp \
       $(SRC_PREFIX)/custom.cpp \
       $(SRC_PREFIX)/derived.cpp \
       $(SRC_PREFIX)/field.cpp \
       $(SRC_PREFIX)/fileio.cpp \
       $(SRC_PREFIX)/format.cpp \
       $(SRC_PREFIX)/handler.cpp \
       $(SRC_PREFIX)/persist.cpp \
       $(SRC_PREFIX)/std.cpp \
       $(SRC_PREFIX)/store.cpp \
       $(SRC_PREFIX)/string.cpp \
       $(SRC_PREFIX)/table.cpp \
       $(SRC_PREFIX)/univ.cpp \
       $(SRC_PREFIX)/view.cpp \
       $(SRC_PREFIX)/viewx.cpp 
