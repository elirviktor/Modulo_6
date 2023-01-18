import requests
import random
import time

TOKEN = 'BBFF-lEtxZ5IHfrLTUF1iR406z9gXAuOEHG'
DEVICE_LABEL = 'machine-Italo'
VARIABLE_LABEL = 'temperature'

def main():    
    host = 'http://industrial.api.ubidots.com'
    url = '{}/api/v1.6/devices/{}/{}/values/?page_size=3'.format(host, DEVICE_LABEL, VARIABLE_LABEL)
    headers = {'X-Auth-Token': TOKEN, 'Content-Type': 'application/json'}
   
    response = requests.get(url=url, headers=headers)
    data = response.json()
    
    status = response.status_code
    print(response)
    if status >= 400:
        print(status)
        print('[ERROR] No se pudo obtener los datos')
    else:
        print(data)
        

if __name__ == '__main__':
    i = 0
    while i < 20:
        main()
        time.sleep(2)
        i += 1