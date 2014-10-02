srcs = main.c syslink.c pseudo.c


include $(SYSLINK)/products.mak
-include $(addprefix bin/$(PROFILE)/obj/,$(patsubst %.c,%.ov5T.dep,$(srcs)))


objs = $(addprefix bin/$(PROFILE)/obj/,$(patsubst %.c,%.ov5T,$(srcs)))
libs = $(SYSLINK)/packages/ti/syslink/lib/syslink.a_$(PROFILE)

PKGPATH := $(SYSLINK)/packages
PKGPATH := $(PKGPATH)+$(BIOS_INSTALL_DIR)/packages
PKGPATH := $(PKGPATH)+$(IPC_INSTALL_DIR)/packages


#Common:
#	@echo "#"
#	@echo "# Making $@ in common/usr/Linux"
#	$(MAKE) -C common/usr/Linux $(SAMPLE_VARS)

all:#Common
	@$(ECHO) "#"
	@$(ECHO) "# Making $@ ..." $(SAMPLE_VARS)
	$(MAKE) -I$(SYSLINK)/packages PROFILE=debug  sh-electric
	$(MAKE) -I$(SYSLINK)/packages PROFILE=release  sh-electric

help:
	@$(ECHO) "make                  # build executables"
	@$(ECHO) "make clean            # clean everything"

install:
	@$(ECHO) "#"
	@$(ECHO) "# Making $@ ..."
	@$(MKDIR) $(EXEC_DIR)/debug
	$(CP) bin/debug/sh-electric $(EXEC_DIR)/debug
	@$(MKDIR) $(EXEC_DIR)/release
	$(CP) bin/release/sh-electric $(EXEC_DIR)/release

clean::
	$(RMDIR) bin


#
#  ======== rules ========
#
sh-electric: bin/$(PROFILE)/sh-electric
bin/$(PROFILE)/sh-electric: $(objs) $(libs) syslinksamples.o_debug slaveloader.o_debug
	@$(ECHO) "#"
	@$(ECHO) "# Making $@ ..."
	$(LD) $(LDFLAGS) -o $@ $^ $(LDLIBS)

bin/$(PROFILE)/obj/%.ov5T: %.c
	@$(ECHO) "#"
	@$(ECHO) "# Making $@ ..."
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $<

#  ======== install validation ========
ifeq (install,$(MAKECMDGOALS))
ifeq (,$(EXEC_DIR))
$(error must specify EXEC_DIR)
endif
endif

#  ======== toolchain macros ========
CC = $(CGT_ARM_PREFIX)gcc -c -MD -MF $@.dep -march=armv5t
AR = $(CGT_ARM_PREFIX)ar cr
LD = $(CGT_ARM_PREFIX)gcc

CPPFLAGS = -D_REENTRANT -Dxdc_target_name__=GCArmv5T \
    -Dxdc_target_types__=gnu/targets/arm/std.h

CFLAGS = -Wall -ffloat-store  -fPIC -Wunused -Dfar= $(CCPROFILE_$(PROFILE)) \
    -I. $(addprefix -I,$(subst +, ,$(PKGPATH)))

LDFLAGS = $(LDPROFILE_$(PROFILE)) -Wall -Wl,-Map=$@.map
LDLIBS = -lpthread -lc -lrt

CCPROFILE_debug = -ggdb -D DEBUG
CCPROFILE_release = -O3 -D NDEBUG

LDPROFILE_debug = -ggdb
LDPROFILE_release = -O3

#  ======== standard macros ========
ifneq (,$(wildcard $(XDC_INSTALL_DIR)/bin/echo.exe))
    # use these on Windows
    CP      = $(XDC_INSTALL_DIR)/bin/cp
    ECHO    = $(XDC_INSTALL_DIR)/bin/echo
    MKDIR   = $(XDC_INSTALL_DIR)/bin/mkdir -p
    RM      = $(XDC_INSTALL_DIR)/bin/rm -f
    RMDIR   = $(XDC_INSTALL_DIR)/bin/rm -rf
else
    # use these on Linux
    CP      = cp
    ECHO    = echo
    MKDIR   = mkdir -p
    RM      = rm -f
    RMDIR   = rm -rf
endif

#  ======== create output directories ========
ifneq (clean,$(MAKECMDGOALS))
ifneq (,$(PROFILE))
ifeq (,$(wildcard bin/$(PROFILE)/obj))
    $(shell $(MKDIR) -p bin/$(PROFILE)/obj)
endif
endif
endif
