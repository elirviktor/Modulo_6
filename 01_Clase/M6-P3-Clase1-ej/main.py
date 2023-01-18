import json
import time
from paho.mqtt import client as mqtt_client


broker = "test.mosquitto.org"
port = 1883
topic = "pucv/iot/grupo5"

def connect_mqtt():

    client = mqtt_client.Client()
    client.connect(broker, port)
    return client

def publish(client):
    counter = 0
    while True:
        time.sleep(3)
        data = {
            "counter": counter,
            "type": "contador",
            "paralelo": 3.0,
            "otra": "llave",
            "temperatura": 25.34
        }

        msg = json.dumps(data)
        #msg = f"contador: { counter }"
        client.publish(topic, msg)
        counter += 1


def run():
    client = connect_mqtt()
    client.loop_start()
    publish(client)

if __name__ == '__main__':
     run()


