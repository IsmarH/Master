import serial
import time
import numpy as np
import sys

arduino = serial.Serial(port='COM3', baudrate=115200*2, write_timeout=2)   #either x1 or x2 works

print("loading")
x_test = np.load('x_valid.npy')
y_test = np.load('y_valid.npy')
print("loaded")


resultsFile = open("Test for full testing data target 0.txt", "w")
#resultsFile = open("testing.txt", "w")
counter = 1
for i in range(len(y_test)):
    if y_test[i][2] == 0:
         #print(y_valid[i])
         for j in range(len(x_test[i])):
           #print (x[i][j][2])
            #use these values for a non quantized model
            x = round(((x_test[i][j][0]).item()),6)
            y = round(((x_test[i][j][1]).item()),6)
            z = round(((x_test[i][j][2]).item()),6)
            #use these values for quantized model
            #x = int((x_test[i][j][0]).item())
            #y = int((x_test[i][j][1]).item())
            #z = int((x_test[i][j][2]).item())
            
            message = str(counter) + ',' + str(x) + ',' + str(y) + ',' + str(z) + '\n'
            print(message)
            message = bytes(str(message), 'utf-8')
            arduino.write(message)
            
            if counter%50==0: 
                time.sleep(0.1)
            else: 
                time.sleep(0.01) 
                counter += 1
            resultsFile.write("x=" + str(x) + ", y=" + str(y) + ", z=" + str(z)+ "\n") 
            while arduino.in_waiting:
                response = arduino.readline()
                #print(str(response.decode("utf-8") ))
                resultsFile.write(response.decode("utf-8"))



