import keras

# Import CFAR10 dataset
(x_train, y_train), (x_test, y_test) = keras.datasets.cifar10.load_data()

# Check the dataset shapes and sizes
assert x_train.shape == (50000, 32, 32, 3)
assert x_test.shape == (10000, 32, 32, 3)
assert y_train.shape == (50000, 1)
assert y_test.shape == (10000, 1)

# We are interested in the test set (for the time being)
IMG_S = x_test.shape[0]
IMG_H = x_test.shape[1]
IMG_W = x_test.shape[2]
IMG_C = x_test.shape[3]

x_test = x_test.reshape(IMG_S, IMG_H * IMG_W * IMG_C)

# Number of images we want to dump out of the test set
IMG_N = 2000

src_h = open('src.h', 'w')
s = 0
src_h.write("#ifndef SRC_H\n")
src_h.write("#define SRC_H\n")
src_h.write("unsigned char src_data[{}] = ".format(IMG_N * IMG_H * IMG_W * IMG_C))
src_h.write("{\n")
for idx in range(IMG_N):
    img = x_test[idx] 
    for pxl in img:
        src_h.write("{}, ".format(pxl))
    src_h.write("\n")
src_h.write("};\n")
src_h.write("#endif\n")
src_h.close()

dst_h = open('dst.h', 'w')
s = 0
dst_h.write("#ifndef DST_H\n")
dst_h.write("#define DST_H\n")
dst_h.write("unsigned char dst_data[{}] = ".format(IMG_N))
dst_h.write("{\n")
for idx in range(IMG_N):
    lbl = y_test[idx][0] 
    dst_h.write("{}, \n".format(lbl))
dst_h.write("};\n")
dst_h.write("#endif\n")
dst_h.close()
