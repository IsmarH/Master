import numpy as np
import matplotlib.pyplot as plt
from matplotlib.pyplot import figure

import seaborn as sn
import pandas as pd

x_valid = np.load('x_valid.npy')
y_valid = np.load('y_valid.npy')
x_test = np.load('x_test.npy')
x_train = np.load('x_train.npy')



print(len(x_test))

for i in range(len(x_valid)): 
    for j in range(len(x_valid[0])): 
        for k in range(len(x_valid[0][0])):
            if x_valid[i][j][k] > 0: 
                x_valid[i][j][k] = round(x_valid[i][j][k] * 1.634)
            else: 
                x_valid[i][j][k] = round(x_valid[i][j][k] *1.634)



data= x_valid.flatten()
np.save("X_valid_quantized.npy", x_valid)




fig = figure( dpi=300)


fig, ax = plt.subplots()
ax.scatter(range(len(data)),data , s=5, c="blue")
plt.show()
