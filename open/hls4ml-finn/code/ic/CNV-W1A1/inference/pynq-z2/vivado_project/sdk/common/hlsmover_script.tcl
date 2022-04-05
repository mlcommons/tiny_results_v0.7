setws .
if { $::argc == 1 } {
    set accname [lindex $::argv 0]
    createhw -name $accname\_hlsmover_platform -hwspec hdf/$accname\_hlsmover_design_wrapper.hdf
    createapp -name $accname\_hlsmover_standalone -app {Hello World} -proc ps7_cortexa9_0 -hwproject $accname\_hlsmover_platform -os standalone
	createapp -name $accname\_fsbl -app {Zynq FSBL} -proc ps7_cortexa9_0 -hwproject $accname\_hlsmover_platform -os standalone
	configapp -app $accname\_hlsmover_standalone build-config release
	configapp -app $accname\_hlsmover_standalone -add linker-misc {-Wl,--defsym=_HEAP_SIZE=0x100000}
    configapp -app $accname\_hlsmover_standalone -add linker-misc {-Wl,--defsym=_STACK_SIZE=0x100000}
}
