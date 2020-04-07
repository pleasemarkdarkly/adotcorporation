#
#  Dan Bolstad - danb@iobjects.com
#  Dan Conti - danc@iojbects.com
#
#  eCos kernel library makefile
#

ifndef TARGET
	error??
endif

include targets

ifeq ($(OSTYPE),cygwin)
        MLTCONV_EXE := mltconv.exe
else
        MLTCONV_EXE := mltconv
endif

INSTALL_PREFIX = $(TARGET)
TARGET_DIR = $(PWD)/$(ECOS_REPOSITORY)/builds/$(TARGET)/
MLT_FILE_PREFIX = $(patsubst %.mlt,%,$(MLT_FILE))
BUILD_DIR = $(TARGET_DIR)$(INSTALL_PREFIX)_build
INSTALL_DIR = $(TARGET_DIR)$(INSTALL_PREFIX)_install
INSTALL_MLT_DIR = $(INSTALL_DIR)/include/pkgconf
MLT_DIR = $(TARGET_DIR)$(INSTALL_PREFIX)_mlt
MLTCONV_DIR = $(PWD)/mltconv/
MLTCONV_PATH = $(PWD)/mltconv/$(MLTCONV_EXE)
CONFIG_FILE = $(TARGET).ecc
SRC_REPOSITORY = $(PWD)/$(ECOS_REPOSITORY)/packages/

all:
	mkdir -p $(BUILD_DIR)
	@echo 
	@echo ---- CONTRUCT THE BUILD AND INSTALL TREES
	cd $(BUILD_DIR) && \
	ecosconfig --srcdir=$(SRC_REPOSITORY) --config=$(TARGET_DIR)$(CONFIG_FILE) --prefix=$(INSTALL_DIR) tree
	@echo 
	@echo ---- BUILD MLTCONV
	cd $(MLTCONV_DIR) && \
	make
	@echo 
	@echo ---- USE CUSTOM MLT FILES
	cd $(MLTCONV_DIR) && \
	$(MLTCONV_PATH) $(MLT_DIR)/$(MLT_FILE) $(INSTALL_MLT_DIR)
	@echo 
	@echo ---- BUILD THE KERNEL LIBRARIES
	cd $(BUILD_DIR) && \
	make
	@echo $@ finished

clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(INSTALL_DIR)
	@echo $@ finished