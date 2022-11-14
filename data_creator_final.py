import os
data = []
files_to_read = os.listdir()

for file_to_read in files_to_read: 
    if(not(file_to_read.endswith(".py")) and not(file_to_read.endswith(".ipynb")) and "data" not in file_to_read and "nicla" not in file_to_read and "MW" not in file_to_read):

        with open(file_to_read, "r") as f:
            lines = f.readlines()   
        
            for idx, line in enumerate(lines):  # pylint: disable=unused-variable,redefined-outer-name
                
                if len(line) > 15:
                #print(line)
                    data.append(line)
f = open("data-negative_new.txt", "w")
for line in data: 
    f.write(line)
i=0
min = 0
max = 0
new = open("data_negative_quantized_new.txt", "w")
with open ("data-negative_new.txt","r") as f: 
    lines = f.readlines()
    for line in lines: 
        X=''
        for number in line: 
            if not(number=="," or number == "\n"): 
                X+=number
            if number ==',': 
                if (float(X)>=0):
                    #new.write(str(round((float(X)*63)/1000))+",")
                    new.write(str(round(float(X)*0.06225))+",")
                if (float(X)<0):
                    new.write(str(round(float(X)*0.06274))+",")
                
                
                if float(X)>max: 
                    max = float(X)
                if float(X)<min: 
                    min = float(X)
                X=''
              
            if number == "\n": 
                 if (float(X)>=0):

                    #new.write(str(round((float(X)*63)/1000))+",")
                    new.write(str(round(float(X)*0.06225))+"\n")
                 if (float(X)<0):
                    new.write(str(round(float(X)*0.06274))+"\n")
                
                # new.write(str(round((float(X)*63)/1000))+"\n")
                
                 if float(X)>max: 
                        max = float(X)
                 if float(X)<min: 
                    min = float(X)
                 X=''
print ("min is: " + str(min) + " and max is: " + str(max))
                
            
                
                

            


                
