import paho.mqtt.client as mqtt
import requests

# Initialize a dictionary to store the transformed data
transformed_data = {}
id_counter = 1

def on_connect(client, userdata, flags, rc):
    print("Connected to broker " + str(rc))
    client.subscribe("dht11")

def on_message(client, userdata, msg):
    global id_counter
    print(f"message on topic {msg.topic}: {msg.payload.decode()}")

    # Parse the received payload
    payload = msg.payload.decode()

    # Splitting the payload to extract temperature and humidity
    values = payload.split(" : ")
    value_type = values[0]
    value = values[1]

    # Update the transformed data dictionary
    transformed_data[value_type] = f"{value_type} : {value}"

    # Check if both temperature and humidity data exist, then send to the server
    if "Temperature" in transformed_data and "Humidity" in transformed_data:
        transformed_data["id"] = id_counter
        send_to_json_server()
        id_counter += 1

def send_to_json_server():
    global transformed_data

    # Create the final payload for the server
    final_data = [transformed_data]

    url = "http://172.20.10.3:3000/dht11"  # Change the URL accordingly

    # POST request to the JSON server
    headers = {'Content-type': 'application/json'}
    response = requests.post(url, json=final_data, headers=headers)

    # Reset transformed_data after sending
    transformed_data = {}


client = mqtt.Client()

client.on_connect = on_connect
client.on_message = on_message

client.connect("172.20.10.3", port=1883)

client.loop_forever()