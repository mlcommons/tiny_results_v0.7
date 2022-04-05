# See
# https://www.xilinx.com/html_docs/xilinx2019_1/SDK_Doc/xsct/intro/xsct_introduction.html

setws .
if { $::argc == 1 } {
    set accname [lindex $::argv 0]
    createhw -name $accname\_platform -hwspec hdf/$accname\_design_wrapper.hdf
    createapp -name $accname\_standalone -app {Hello World} -proc microblaze_mcu -hwproject $accname\_platform -os standalone 
    configapp -app $accname\_standalone -add define-compiler-symbols {__USE_OCM__}
    configapp -app $accname\_standalone build-config release
}
