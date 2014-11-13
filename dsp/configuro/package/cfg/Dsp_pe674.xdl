/*
 * Do not modify this file; it is automatically generated from the template
 * linkcmd.xdt in the ti.targets.elf package and will be overwritten.
 */

/*
 * put '"'s around paths because, without this, the linker
 * considers '-' as minus operator, not a file name character.
 */


-l"/home/liao/src/syslinkapp/dsp/configuro/package/cfg/Dsp_pe674.oe674"
-l"/home/liao/src/syslink/packages/ti/syslink/ipc/rtos/lib/release/ti.syslink.ipc.rtos.ae674"
-l"/opt/ti/ipc_1_24_02_27/packages/ti/sdo/ipc/lib/nonInstrumented_e674/ipc/ipc.lib"
-l"/opt/ti/bios_6_33_02_31/packages/ti/sysbios/lib/nonInstrumented_e674/sysbios/sysbios.lib"
-l"/opt/ti/xdctools_3_23_01_43/packages/ti/targets/rts6000/lib/ti.targets.rts6000.ae674"
-l"/opt/ti/xdctools_3_23_01_43/packages/ti/targets/rts6000/lib/boot.ae674"

--retain="*(xdc.meta)"


--args 0x64
-heap  0x0
-stack 0x1000

MEMORY
{
    IROM (RX) : org = 0x11700000, len = 0x100000
    IRAM (RWX) : org = 0x11800000, len = 0x38000
    L3_CBA_RAM (RWX) : org = 0x80000000, len = 0x20000
    SR_0 (RWX) : org = 0xc2000000, len = 0x10000
    DSP_PROG (RWX) : org = 0xc3000000, len = 0x800000
}

/*
 * Linker command file contributions from all loaded packages:
 */

/* Content from xdc.services.global (null): */

/* Content from xdc (null): */

/* Content from xdc.corevers (null): */

/* Content from xdc.shelf (null): */

/* Content from xdc.services.spec (null): */

/* Content from xdc.services.intern.xsr (null): */

/* Content from xdc.services.intern.gen (null): */

/* Content from xdc.services.intern.cmd (null): */

/* Content from xdc.bld (null): */

/* Content from ti.targets (null): */

/* Content from ti.targets.elf (null): */

/* Content from xdc.rov (null): */

/* Content from xdc.runtime (null): */

/* Content from ti.targets.rts6000 (null): */

/* Content from ti.sysbios.interfaces (null): */

/* Content from ti.sysbios.family (null): */

/* Content from ti.sysbios (null): */

/* Content from ti.sysbios.hal (null): */

/* Content from ti.sysbios.knl (null): */

/* Content from ti.sysbios.gates (null): */

/* Content from ti.sysbios.family.c64p (null): */

/* Content from xdc.services.getset (null): */

/* Content from xdc.runtime.knl (null): */

/* Content from ti.sdo.ipc.family (null): */

/* Content from ti.sdo.utils (null): */

/* Content from ti.sdo.ipc.interfaces (null): */

/* Content from ti.sysbios.syncs (null): */

/* Content from ti.sysbios.heaps (null): */

/* Content from ti.sysbios.timers.timer64 (null): */

/* Content from ti.sysbios.xdcruntime (null): */

/* Content from ti.sysbios.family.c62 (null): */

/* Content from ti.sysbios.family.c64p.primus (null): */

/* Content from ti.sysbios.utils (null): */

/* Content from ti.catalog.c6000 (null): */

/* Content from ti.catalog (null): */

/* Content from ti.catalog.peripherals.hdvicp2 (null): */

/* Content from xdc.platform (null): */

/* Content from xdc.cfg (null): */

/* Content from ti.catalog.arm.peripherals.timers (null): */

/* Content from ti.catalog.arm (null): */

/* Content from ti.platforms.evmOMAPL138 (null): */

/* Content from ti.sdo.ipc (ti/sdo/ipc/linkcmd.xdt): */

SECTIONS
{
    ti.sdo.ipc.SharedRegion_0:  { . += 0x10000;} run > 0xc2000000, type = NOLOAD

    ti_sdo_ipc_init: load > DSP_PROG, type = NOINIT 
}


/* Content from ti.sdo.ipc.family.da830 (null): */

/* Content from ti.sdo.ipc.notifyDrivers (null): */

/* Content from ti.sdo.ipc.transports (null): */

/* Content from ti.sdo.ipc.nsremote (null): */

/* Content from ti.sdo.ipc.heaps (null): */

/* Content from ti.syslink.ipc.rtos (ti/syslink/ipc/rtos/linkcmd.xdt): */


        -eti_sysbios_family_c64p_Hwi0

/* Add the flags needed for SysLink ELF build. */
    --dynamic
    --retain=_Ipc_ResetVector

/* Content from ti.sdo.ipc.gates (null): */

/* Content from configuro (null): */


/*
 * symbolic aliases for static instance objects
 */
xdc_runtime_Startup__EXECFXN__C = 1;
xdc_runtime_Startup__RESETFXN__C = 1;
TSK_idle = ti_sysbios_knl_Task_Object__table__V + 68;

SECTIONS
{
    .text: load >> DSP_PROG
    .ti.decompress: load > DSP_PROG
    .stack: load > DSP_PROG
    GROUP: load > DSP_PROG
    {
        .bss:
        .neardata:
        .rodata:
    }
    .cinit: load > DSP_PROG
    .pinit: load >> DSP_PROG
    .init_array: load > DSP_PROG
    .const: load >> DSP_PROG
    .data: load >> DSP_PROG
    .fardata: load >> DSP_PROG
    .switch: load >> DSP_PROG
    .sysmem: load > DSP_PROG
    .far: load >> DSP_PROG
    .args: load > DSP_PROG align = 0x4, fill = 0 {_argsize = 0x64; }
    .cio: load >> DSP_PROG
    .ti.handler_table: load > DSP_PROG
    .vecs: load >> DSP_PROG
    xdc.meta: load >> DSP_PROG, type = COPY

}
