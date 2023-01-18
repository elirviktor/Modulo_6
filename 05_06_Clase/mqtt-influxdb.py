#MQTT BROKER MOSQUITO
from paho.mqtt import client as mqtt_client
import json
#NFLUXDB IMPORT
import random
import influxdb_client
import os
from datetime import datetime
import time
from influxdb_client import Point
from influxdb_client.client.write_api import SYNCHRONOUS
#MQTT BROKER MOSQUITO CONFIGURACION
broker = "test.mosquitto.org"
port = 1883
topic = "pucv/iot/m6/p3/g5"
#INFLUXDB CONFIGURACION
token = "99W6voDxWt_Ru7rF6mlyh5ix645GCoA2QT6kcz5W3saXSXiPW-5EdPvAs-5akmdkp0Pruwpsy94zX5hHwZJH0A=="
org = "solinfo2010@gmail.com"
url = "https://eastus-1.azure.cloud2.influxdata.com"
client = influxdb_client.InfluxDBClient(url=url, token=token, org=org)
bucket = "subgrupo5"
write_api = client.write_api(write_options=SYNCHRONOUS)

#MQTT BROKER MOSQUITO FUNCIONES
def connect_mqtt():
    client = mqtt_client.Client()
    client.connect(broker, port)
    return client

i = 0
def subscribe(client):

    def custom_on_message(client, userdata, msg):
        global i
        try:
            
            data = json.loads(msg.payload)    
            
            while True:
                
                data_point = Point("sensorGen_")\
                .tag("device_id", data['device_id'])\
                .field("Temperatura", float(data['Temperatura']))\
                .field("Humedad", float(data['Humedad']))

                write_api.write(bucket=bucket, org=org, record=data_point)
                print(data_point)
                #print(f"Point {i}")
                #i += 1
                #time.sleep(5)

        except:
            i = 0

    client.subscribe(topic)
    client.on_message = custom_on_message

def run():
    client = connect_mqtt()
    subscribe(client)
    client.loop_forever()


# Press the green button in the gutter to run the script.
if __name__ == '__main__':
    run()






