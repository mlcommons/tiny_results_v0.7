# See
# https://www.xilinx.com/html_docs/xilinx2019_1/SDK_Doc/xsct/intro/xsct_introduction.html

setws .
if { $::argc == 1 } {
    set accname [lindex $::argv 0]
    createhw -name $accname\_platform -hwspec hdf/$accname\_design_wrapper.hdf
    createapp -name $accname\_standalone -lang c++ -app {Empty Application} -proc microblaze_mcu -hwproject $accname\_platform -os standalone
	# Uncommment the line below if building for Energy Measurements! Otherwise, leave disabled
	#createapp -name $accname\_bootloader -lang c -app {SREC SPI Bootloader} -proc microblaze_mcu -hwproject $accname\_platform -os standalone
    configapp -app $accname\_standalone build-config release
    configapp -app $accname\_standalone -add linker-misc {-Wl,--defsym=_HEAP_SIZE=0x100000}
    configapp -app $accname\_standalone -add linker-misc {-Wl,--defsym=_STACK_SIZE=0x100000}
}
