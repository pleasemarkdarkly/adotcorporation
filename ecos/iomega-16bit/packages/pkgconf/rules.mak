#=============================================================================
#
#    rules.mak
#
#    Generic rules for inclusion by all package makefiles.
#
#=============================================================================
#####COPYRIGHTBEGIN####
#                                                                          
# -------------------------------------------                              
# The contents of this file are subject to the Red Hat eCos Public License 
# Version 1.1 (the "License"); you may not use this file except in         
# compliance with the License.  You may obtain a copy of the License at    
# http://www.redhat.com/                                                   
#                                                                          
# Software distributed under the License is distributed on an "AS IS"      
# basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the 
# License for the specific language governing rights and limitations under 
# the License.                                                             
#                                                                          
# The Original Code is eCos - Embedded Configurable Operating System,      
# released September 30, 1998.                                             
#                                                                          
# The Initial Developer of the Original Code is Red Hat.                   
# Portions created by Red Hat are                                          
# Copyright (C) 1998, 1999, 2000 Red Hat, Inc.                             
# All Rights Reserved.                                                     
# -------------------------------------------                              
#                                                                          
#####COPYRIGHTEND####
#=============================================================================
#####DESCRIPTIONBEGIN####
#
# Author(s):     jld
# Contributors:  bartv
# Date:          1999-11-04
# Purpose:       Generic rules for inclusion by all package makefiles
# Description:   
#
#####DESCRIPTIONEND####
#=============================================================================

.PHONY: default build clean tests headers mlt_headers

# include any dependency rules generated previously
ifneq ($(wildcard *.deps),)
include $(wildcard *.deps)
endif

# pattern matching rules to generate a library object from source code
# object filenames are prefixed to avoid name clashes
# a single dependency rule is generated (file extension = ".o.d")
%.o.d : %.c
	@mkdir -p $(dir $@)
	$(CC) -c $(INCLUDE_PATH) -I$(dir $<) $(CFLAGS) -Wp,-MD,$(@:.o.d=.tmp) -o $(dir $@)$(OBJECT_PREFIX)_$(notdir $(@:.o.d=.o)) $<
	@echo $@ ':' $< '\' > $@
	@tail +2 $(@:.o.d=.tmp) >> $@
	@echo >> $@
	@rm $(@:.o.d=.tmp)

%.o.d : %.cxx
	@mkdir -p $(dir $@)
	$(CC) -c $(INCLUDE_PATH) -I$(dir $<) $(CFLAGS) -Wp,-MD,$(@:.o.d=.tmp) -o $(dir $@)$(OBJECT_PREFIX)_$(notdir $(@:.o.d=.o)) $<
	@echo $@ ':' $< '\' > $@
	@tail +2 $(@:.o.d=.tmp) >> $@
	@echo >> $@
	@rm $(@:.o.d=.tmp)

%.o.d : %.S
	@mkdir -p $(dir $@)
	$(CC) -c $(INCLUDE_PATH) -I$(dir $<) $(CFLAGS) -Wp,-MD,$(@:.o.d=.tmp) -o $(dir $@)$(OBJECT_PREFIX)_$(notdir $(@:.o.d=.o)) $<
	@echo $@ ':' $< '\' > $@
	@tail +2 $(@:.o.d=.tmp) >> $@
	@echo >> $@
	@rm $(@:.o.d=.tmp)

# pattern matching rules to generate a test object from source code
# object filenames are not prefixed
# a single dependency rule is generated (file extension = ".d")
%.d : %.c
	@mkdir -p $(dir $@)
	$(CC) -c $(INCLUDE_PATH) -I$(dir $<) $(CFLAGS) -Wp,-MD,$(@:.d=.tmp) -o $(@:.d=.o) $<
	@echo $@ ':' $< '\' > $@
	@tail +2 $(@:.d=.tmp) >> $@
	@echo >> $@
	@rm $(@:.d=.tmp)

%.d : %.cxx
	@mkdir -p $(dir $@)
	$(CC) -c $(INCLUDE_PATH) -I$(dir $<) $(CFLAGS) -Wp,-MD,$(@:.d=.tmp) -o $(@:.d=.o) $<
	@echo $@ ':' $< '\' > $@
	@tail +2 $(@:.d=.tmp) >> $@
	@echo >> $@
	@rm $(@:.d=.tmp)

%.d : %.S
	@mkdir -p $(dir $@)
	$(CC) -c $(INCLUDE_PATH) -I$(dir $<) $(CFLAGS) -Wp,-MD,$(@:.d=.tmp) -o $(@:.d=.o) $<
	@echo $@ ':' $< '\' > $@
	@tail +2 $(@:.d=.tmp) >> $@
	@echo >> $@
	@rm $(@:.d=.tmp)

# rule to generate a test executable from object code
$(PREFIX)/tests/$(PACKAGE)/%$(EXEEXT): %.d $(wildcard $(PREFIX)/lib/target.ld) $(wildcard $(PREFIX)/lib/*.[ao])
	@mkdir -p $(dir $@)
ifneq ($(IGNORE_LINK_ERRORS),)
	-$(CC) $(LDFLAGS) -L$(PREFIX)/lib -Ttarget.ld -o $@ $(<:.d=.o)
else
	$(CC) $(LDFLAGS) -L$(PREFIX)/lib -Ttarget.ld -o $@ $(<:.d=.o)
endif

# rule to generate all tests and create a dependency file "tests.deps" by
# concatenating the individual dependency rule files (file extension = ".d")
# generated during compilation
tests: tests.stamp

TESTS := $(TESTS:.cxx=)
TESTS := $(TESTS:.c=)
TESTS := $(TESTS:.S=)
tests.stamp: $(foreach target,$(TESTS),$(target).d $(PREFIX)/tests/$(PACKAGE)/$(target)$(EXEEXT))
ifneq ($(strip $(TESTS)),)
	@cat $(TESTS:=.d) > $(@:.stamp=.deps)
endif
	@touch $@

# rule to clean the build tree
clean:
	@find . -type f -not -name makefile -print0 | xargs -0 rm -f

# rule to copy MLT files
mlt_headers:
ifneq ($(strip $(MLT)),)
	@cp -u $(MLT) $(PREFIX)/include/pkgconf
	@chmod u+w $(PREFIX)/include/pkgconf/mlt*.*
endif

# end of file
