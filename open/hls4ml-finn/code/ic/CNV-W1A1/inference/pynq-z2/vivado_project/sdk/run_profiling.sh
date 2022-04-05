echo "INFO: Remember to log serial output:"
echo "INFO: sudo minicom -D /dev/ttyUSB1 -C minicom_out.log"
echo -n "INFO: Press any key to start: "
read dummy

for i in $(seq 1 1 99); do
	make clean hlsmover-sdk  && xsct program_hlsmover.tcl $i
done
for i in $(seq 100 2 200); do
	make clean hlsmover-sdk  && xsct program_hlsmover.tcl $i
done
for i in $(seq 210 10 500); do
	make clean hlsmover-sdk  && xsct program_hlsmover.tcl $i
done
for i in $(seq 525 25 2000); do
	make clean hlsmover-sdk  && xsct program_hlsmover.tcl $i
done


