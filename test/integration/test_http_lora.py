import requests
import time

from enum import Enum, auto

class CommandEnum(Enum):
    INVALID_OPERATION            = 0
    ACK_COMMAND                  = 1
    READ_COMMAND                 = 2
    READ_RESPONSE_COMMAND        = 3
    WRITE_COMMAND                = 4
    READ_RESPONSE_SECURE_COMMAND = 101
    WRITE_SECURE_COMMAND         = 102
    READ_SECURE_COMMAND          = 103

    def __str__(self):
        # Optional: Custom string representation for better debugging/logging
        return f'{self.name} ({self.value})'

def read_data_using_lora(hostname: str, address: int):
    LORA_TRANSMIT_MEMORY_AREA = 5
    LORA_RECEIVE_MEMORY_AREA = 6
    DELAY = 1
    for memory_area in range(1, 5, 1):
        json_data = {
            'command': CommandEnum.READ_COMMAND.value,
            'address': address,
            'memory_area': memory_area,
            'payload': "{\"mqtt_uri\":\"banana\"}"
        }
        try:
            post_response = requests.post(f"http://{hostname}/get_area?id={LORA_TRANSMIT_MEMORY_AREA}", json=json_data)
            post_response.raise_for_status()
            print('POST Response Status Code:', post_response.status_code)
            print('POST Response JSON:', post_response)
        except requests.exceptions.RequestException as e:
            print('POST Request Error:', e)
        time.sleep(DELAY)
        try:
            get_response = requests.get(f"http://{hostname}/get_area?id={LORA_RECEIVE_MEMORY_AREA}")
            get_response.raise_for_status() 
            print('GET Response Status Code:', get_response.status_code)
            print('GET Response JSON:', get_response.json())
        except requests.exceptions.RequestException as e:
            print('GET Request Error:', e)
        time.sleep(DELAY)

def main():
    time.sleep(5)
    for i in range(5000):
        read_data_using_lora("192.168.3.51", 0x00)

if __name__ == "__main__":
    main()