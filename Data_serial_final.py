import os
import serial
import time
arduino = serial.Serial(port='COM3', baudrate=115200*2, write_timeout=3)   #either x1 or x2 works


file = "testing data magic wand 3.txt"

resultsFile = open("Test for full testing data target 3.txt", "w")
counter = 1
with open(file, "r") as f:
            lines = f.readlines()   
            for idx, line in enumerate(lines):  # pylint: disable=unused-variable,redefined-outer-name
                message = str(counter) +"," + str(line) 
                
                line = line.split(",")
                print(message)
                message = bytes(message, 'utf-8')
                arduino.write(message)
                if counter%128==0: 
                    time.sleep(0.2)
                else: 
                    time.sleep(0.01) 
                counter+=1
                
                resultsFile.write("x=" +line[0] + ", y=" + line[1] + ", z=" + line[2]+"\n") 
                while arduino.in_waiting:   
                    response = arduino.readline()
                    print(str(response.decode("utf-8") ))
                    resultsFile.write(response.decode("utf-8"))
                



