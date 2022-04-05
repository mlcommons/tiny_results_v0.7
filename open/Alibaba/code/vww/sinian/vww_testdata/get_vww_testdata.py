import numpy as np
import os
import cv2

#  ============= extract the 1000 test images  ===========
base_dir = "."


non_person_dir =  base_dir + "vw_coco2014_96/non_person/"
person_dir =  base_dir + "vw_coco2014_96/person/"
y_label_path = base_dir + "y_labels.csv"
test_images_dir  =  base_dir + "test_images_1000/"
test_bin_dir =  base_dir + "vww01/"


os.makedirs(test_images_dir, exist_ok=True)
os.makedirs(test_bin_dir, exist_ok=True)

with open(y_label_path, 'r') as f:
    content = f.readlines()


for i, line in enumerate(content):
    name = "COCO_val2014_" + line[:-9] + ".jpg" # COCO_val2014_000000343053.jpg
    if i < 500:
        os.system("cp " + person_dir + name + " " + test_images_dir  )
    else: # 500-1000
        os.system("cp " + non_person_dir+ name + " " + test_images_dir)


# ============== convert jpeg image to bin ===============
l = os.listdir(test_images_dir)
for i,name in enumerate(l):
    source_img_path = test_images_dir + l[i]
    dest_img_path = test_bin_dir + l[i][13:-4] + ".bin"

    img = cv2.imread(source_img_path)
    img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)    # from BGR to RGB
    img = img.astype(np.uint8)
    img_exp = np.expand_dims(img, axis=0)         # add axis 0,  (1,96,96,3)
    img_exp = np.transpose(img_exp, (0, 3, 1, 2)) # from NHWC to NCHW (1,3,96,96)
    img_exp.tofile(dest_img_path)


