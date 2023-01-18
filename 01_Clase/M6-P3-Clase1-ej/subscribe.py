import json
from paho.mqtt import client as mqtt_client


broker = "test.mosquitto.org"
port = 1883
topic = "pucv/iot/grupo5"

def connect_mqtt():

    client = mqtt_client.Client()
    client.connect(broker, port)
    return client


def subscribe(client):
    def custom_on_message(client, userdata, msg):
        try:
            data = json.loads(msg.payload)
        except:
            print("error")
            
    client.subscribe(topic)
    client.on_message = custom_on_message


def run():
    client = connect_mqtt()
    subscribe(client)
    client.loop_forever()

if __name__ == '__main__':
     run()
