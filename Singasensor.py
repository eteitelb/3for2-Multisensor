
# coding: utf-8

# In[2]:

import pandas as pd
import datetime
import time
from urllib2 import Request, urlopen, URLError
import csv
import json
    
starttime = datetime.datetime.now().strftime('%m_%d_%Y_%H_%M_%S')
lengthofstorage = 10000 #length of time to collect data (in sec)
frequency = 30 #time between readings
    
print "Starting trending at "+str(starttime)+" for "+str(lengthofstorage)+" seconds at "+str(frequency)+" second intervals"
    
filename = starttime+"KitCalibration.csv"
            #set your own zip code
#"54ff6e066678574930451067": "DALE",  
    
deviceID = {"54ff6e066678574930451067": "DALE","54ff6c066672524838271267" : "GANZHI", "54ff6f066672524834591267" : "CLETUS", "54ff69066672524819131167" : "STARBUCK", "54ff6a066672524834581267" : "TEKANAN"}

#deviceID = {"54ff69066672524819131167" : "STARBUCK"}
#Measurements = ["NOX_voltage", "CO2_voltage", "CO_voltage", "O3_voltage", "PART_voltage", "temp", "hum", "FORM_voltage", "VOC_voltage"]
Measurements = ["NOX_voltage", "CO_voltage", "O3_voltage","FORM_voltage", "VOC_voltage"]
access_token = '5703d71eab3d5437b38bfa5c5be75ee5c0e86e3b'
    
print "Create file named "+filename+" -- you'll only be able to open to see the data once the file is finished"
    
#Open a file and write a line to it each time a value is retreived
with open(filename, "a") as file:
        writer = csv.writer(file, delimiter = ",")
        end = time.time() + lengthofstorage
        index = ('timestamp', 'Device', 'Variable', 'Value')
        writer.writerow(index)
        #The while loop will run until the current time + length of storage
        while True:
            if time.time() > end:
                break
            else:
                
                for i in deviceID.keys():
                    url = 'https://api.spark.io/v1/devices/' + i + '/?access_token=' + access_token
                    try:
                        response = urlopen(url)
                        string = response.read()
                        JSON_response = json.loads(string)
                        value = JSON_response['variables']
                        print value
                    except URLError, e:
                        print 'Got an error code:', e
                        continue
                    for k in value.keys():
                        
                        url = 'https://api.spark.io/v1/devices/' + i + '/' + k + '?access_token=' + access_token
                        #print url
                        response = urlopen(url)
                        string = response.read()
                        JSON_response = json.loads(string)
                        value = JSON_response['result']
    
                        try:
                        
                            #get raw data from request
                            response = urlopen(url)
                            string = response.read()
                            JSON_response = json.loads(string)
                            value = JSON_response['result']    
                    
                            #Set the 'timestamp' to the datatime now (from the server)
                            timestamp = datetime.datetime.now().strftime('%m/%d/%Y %H:%M:%S')
                            all_data =[timestamp, deviceID.get(i), k, value]
                            print "storing value: "
                            print all_data
                    
                            #Write a row to the file
                            writer.writerow(all_data)
                    
                        #if there is an error in reaching the API then throw an error message
                        except URLError, e:
                            print 'Got an error code:', e
                            continue
                    
    
                #put the while loop to sleep for the length between readings
                time.sleep(frequency)


# In[8]:




# In[ ]:



