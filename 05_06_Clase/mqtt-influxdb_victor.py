
import influxdb_client
from influxdb_client.client.write_api import SYNCHRONOUS
from Tools.scripts.dutree import display
from paho.mqtt import client as mqtt_client
import json
import influxdb_client, os, time
from influxdb_client import InfluxDBClient, Point, WritePrecision
from influxdb_client.client.write_api import SYNCHRONOUS


##INFLUXDB_TOKEN =0xHXC6dpFyZ6AoEqAUtyjkq5oy8mw6d-xcZf2lxxP3K2Y4R2saDpjvWyP8lEjR9ibz1HfrVCHIu2-rvRk6GlgA==


broker = "test.mosquitto.org"
port = 1883
topic = "pucv/iot/m6/p3/g3"

token = "xxxxxxx"
org = "solinfo2010@gmail.com"
url = "https://eastus-1.azure.cloud2.influxdata.com"
bucket = "subgrupo5"
client2 = influxdb_client.InfluxDBClient(url=url, token=token, org=org)
write_api = client2.write_api(write_options=SYNCHRONOUS)
json_data = []
df_all = []
df = []

def connect_mqtt():
    client = mqtt_client.Client()
    client.connect(broker, port)
    return client


def subscribe(client):
    def custom_on_message(client, userdata, msg):

        try:

            data = json.loads(msg.payload)
            print(data)
            #info = json.loads(msg.payload)
            #df = json_normalize(data)  # Results contain the required data
            #print(df)
            #data = json.load(file)
            #json_data.append(json_normalize(df))
            ##print(json_data)

            #df_all = pd.DataFrame.from_records(json_data)

            #print(df_all)
            point1 = (
                Point("sensorTemp")\
                .tag("sensor_id", data['sensor_id'])\
                .field("temperatura", data['temperatura'])\
                .field("humedad", data['humedad'])
            )
            #print(point1)
            write_api.write(bucket=bucket, org=org, record=point1)


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
