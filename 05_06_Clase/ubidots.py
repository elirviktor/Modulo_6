import requests
import random
import time

TOKEN = 'xxxxxxx'
DEVICE_LABEL = 'lala'

def main():
    host = 'http://industrial.api.ubidots.com'
    url = '{}/api/v1.6/devices/{}'.format(host, DEVICE_LABEL)
    headers = {'X-Auth-Token': TOKEN, 'Content-Type': 'application/json'}
    payload = {
        "temperature": random.randint(-10, 50),
        "humidity": random.randint(0, 100),
        "position": {"value": 1, "context": {"lat": -33.03190, "lng": -71.591466}}
    }
    response = requests.post(url=url, headers=headers, json=payload)
    status = response.status_code
    if status >= 400:
        print(status)
        print('[ERROR] No se pudo enviar los datos')
    else:
        print('[INFO] Agregado correctamente')

if __name__ == '__main__':
    i = 0
    while i < 20:
        main()
        time.sleep(2)
        i += 1

