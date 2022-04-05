echo "=========== 1. make clean ==========="
make clean

echo "=========== 2. make  ==========="
make

echo "=========== 3. copy necessary files to ../runner dir ==========="
cp io.* ../vww_runner
cp model.* ../vww_runner