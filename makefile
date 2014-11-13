
include $(SYSLINK)/products.mak
SYSLINK_ROOT:=SYSLINK
.PHONY: install

all:
	@$(ECHO) "#"
	@$(ECHO) "# Making $@ ..."
	cp -f $(SYSLINK)/packages/ti/syslink/obj/OMAPL1XX/samples/slaveloader.o_debug ./arm/slaveloader.o_debug
	cp -f $(SYSLINK)/packages/ti/syslink/obj/OMAPL1XX/samples/syslinksamples.o_debug ./arm/syslinksamples.o_debug
	$(MAKE) -C arm all
	$(MAKE) -C dsp all
	#rm dsp.xe674
	#rm sh-electric
	mv -f dsp/bin/debug/dsp.xe674 ./dsp.xe674
	mv -f arm/bin/debug/sh-electric ./sh-electric

help:
	@$(ECHO) "make                  # build executables"
	@$(ECHO) "make clean            # clean everything"
	@$(ECHO) "make install          # stage install folder"


EXEC_DIR=$(CURDIR)/install


SLAVELOADER=$(SYSLINK_INSTALL_DIR)/packages/ti/syslink/bin
SLAVELOADER_DEBUG:=$(SLAVELOADER)/OMAPL1XX/samples/slaveloader_debug
SLAVELOADER:=$(SLAVELOADER)/OMAPL1XX/samples/slaveloader_release

install:
	@$(ECHO) "#"
	@$(ECHO) "# Making $@ ..."
	@$(MKDIR) $(EXEC_DIR)/debug
	$(INSTALL) run.sh $(EXEC_DIR)/debug
	$(CP) $(SLAVELOADER_DEBUG) $(EXEC_DIR)/debug/slaveloader
	@$(MKDIR) $(EXEC_DIR)/release
	$(INSTALL) run.sh $(EXEC_DIR)/release
	$(CP) $(SLAVELOADER) $(EXEC_DIR)/release/slaveloader
	$(MAKE) -C host EXEC_DIR=$(EXEC_DIR) install
	$(MAKE) -C dsp EXEC_DIR=$(EXEC_DIR) install

clean::
	$(RMDIR) install
	$(MAKE) -C arm clean
	$(MAKE) -C dsp clean
	rm dsp.xe674
	rm sh-electric


#  ======== standard macros ========
ifneq (,$(wildcard $(XDC_INSTALL_DIR)/bin/echo.exe))
    # use these on Windows
    CP      = $(XDC_INSTALL_DIR)/bin/cp
    ECHO    = $(XDC_INSTALL_DIR)/bin/echo
    INSTALL = $(XDC_INSTALL_DIR)/bin/cp
    MKDIR   = $(XDC_INSTALL_DIR)/bin/mkdir -p
    RM      = $(XDC_INSTALL_DIR)/bin/rm -f
    RMDIR   = $(XDC_INSTALL_DIR)/bin/rm -rf
else
    # use these on Linux
    CP      = cp
    ECHO    = echo
    INSTALL = install
    MKDIR   = mkdir -p
    RM      = rm -f
    RMDIR   = rm -rf
endif
