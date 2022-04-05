echo "=========== 1. make clean ==========="
make clean

echo "=========== 2. make  ==========="
make

echo "=========== 3. copy necessary files to ../runner dir ==========="
cp io.* ../ad_runner
cp model.* ../ad_runner