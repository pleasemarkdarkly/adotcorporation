MODULE := jukebox
SRC_PREFIX := $(MODULE)/src
HDR_PREFIX := $(MODULE)/include

HDR += $(HDR_PREFIX)/MetakitManager.h $(HDR_PREFIX)/Jukebox.h $(HDR_PREFIX)/JPlaylist.h
SRC += $(SRC_PREFIX)/MetakitManager.cpp $(SRC_PREFIX)/Jukebox.cpp $(SRC_PREFIX)/JPlaylist.cpp 
