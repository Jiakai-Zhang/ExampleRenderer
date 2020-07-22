import cv2
import matplotlib.pyplot as plt

testPath = '../depth.png'


img = cv2.imread(testPath,cv2.IMREAD_ANYDEPTH)
plt.imshow(img)
plt.show()
