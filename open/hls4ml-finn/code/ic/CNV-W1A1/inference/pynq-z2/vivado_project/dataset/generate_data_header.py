import keras

(x_train, y_train), (x_test, y_test) = keras.datasets.cifar10.load_data()
assert x_train.shape == (50000, 32, 32, 3)
assert x_test.shape == (10000, 32, 32, 3)
assert y_train.shape == (50000, 1)
assert y_test.shape == (10000, 1)


IMG_N = x_test.shape[0]
IMG_H = x_test.shape[1]
IMG_W = x_test.shape[2]
IMG_C = x_test.shape[3]

x_test_flat = x_test.reshape(IMG_N, IMG_H * IMG_W * IMG_C)

# First S images in the test set 
IMG_S = 10

f = open("src.h", "w")
f.write("unsigned char src_data[{}] = ".format(IMG_S*IMG_H*IMG_W*IMG_C))
f.write("{\n")
for i in range(IMG_S):
    for x in x_test_flat[i]:
        f.write("{}, ".format(x))
    f.write("\n")
f.write("};")
f.close()

f = open("dst.h", "w")
f.write("unsigned char dst_data[{}] = ".format(IMG_S))
f.write("{\n")
for i in range(IMG_S):
    for y in y_test[i]:
        f.write("{}, ".format(y))
    f.write("\n")
f.write("};")

f.close()

