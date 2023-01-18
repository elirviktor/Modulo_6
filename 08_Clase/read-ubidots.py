import requests
import random
import pandas as pd
import json
from pandas import json_normalize
import time

TOKEN = 'xxxxxx'
DEVICE_LABEL = 'machine-Italo'
temp = 'temperature'

def main():
    host = 'https://industrial.api.ubidots.com/api/v1.6/devices'
    url = '{}/{}/{}/values'.format(host, DEVICE_LABEL, temp)
    print(url)
    headers = {'X-Auth-Token': TOKEN}
    payload = {
        "temperature": random.randint(-10, 50),
        "humidity": random.randint(0, 100),
        "position": {"value": 1, "context": {"lat": -33.03190, "lng": -71.591466}}
    }
    response = requests.get(url=url, headers=headers)
    data = response.json()
    print(data)
    my_json_str = json.dumps(data)
    dict = json.loads(my_json_str)
    # df2 = pd.DataFrame.from_dict(dict, orient="index")
    df2 = json_normalize(dict['results'])
    print(df2)


    status = response.status_code
    if status >= 400:
        print(status)
        print('[ERROR] No se pudo enviar los datos')
    else:
        print('[INFO] Agregado correctamente')

if __name__ == '__main__':
    main()

