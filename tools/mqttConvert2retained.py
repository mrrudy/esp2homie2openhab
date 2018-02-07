import paho.mqtt.client as mqtt

# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
#    client.subscribe("DEBUG/pythonscript")
    client.subscribe("homie/5ccf7f2c12d1/desiredTemp/degrees/set")

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    if msg.retain==False:
        print("addint retain to: "+msg.topic+" "+str(msg.payload))
        client.unsubscribe(msg.topic)
        client.publish(msg.topic, payload=msg.payload, qos=0, retain=True)
        client.subscribe(msg.topic)
    else:
        print("message already has retain flag")
    return;

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.connect("192.168.1.25", 1883, 60)

# Blocking call that processes network traffic, dispatches callbacks and
# handles reconnecting.
# Other loop*() functions are available that give a threaded interface and a
# manual interface.
client.loop_forever()
