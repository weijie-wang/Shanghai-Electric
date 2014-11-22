#
_XDCBUILDCOUNT = 
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = /home/liao/src/syslink/packages;/opt/ti/bios_6_33_02_31/packages;/opt/ti/ipc_1_24_02_27/packages;/opt/ti/xdctools_3_23_01_43/packages
override XDCROOT = /opt/ti/xdctools_3_23_01_43
override XDCBUILDCFG = ./config.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = 
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = /home/liao/src/syslink/packages;/opt/ti/bios_6_33_02_31/packages;/opt/ti/ipc_1_24_02_27/packages;/opt/ti/xdctools_3_23_01_43/packages;/opt/ti/xdctools_3_23_01_43/packages;..
HOSTOS = Linux
endif
