import paho.mqtt.publish as publish
import os, time, glob

class MQTTClient:
    def __init__(self, topic, hostname):
        self.topic = topic
        self.hostname = hostname

    def publish(self, message):
        publish.single(self.topic, message, hostname=self.hostname)

class TemperatureSensor:
    def __init__(self):
        self.base_dir = '/sys/bus/w1/devices/'
        self.device_folder = glob.glob(self.base_dir + '28*')[0]
        self.device_file = self.device_folder + '/w1_slave'

        # Set up stuff
        os.system("modprobe w1-gpio")
        os.system("modprobe w1-therm")


    def readRaw(self):
        f = open(self.device_file, 'r')
        lines = f.readlines()
        f.close()
        return lines


    def readTemp(self):
        """Return temperature in celsius"""
        lines = self.readRaw()
        while lines[0].strip()[-3:] != 'YES':
            time.sleep(0.2)
            lines = self.readRaw()

        equals_pos = lines[1].find('t=')
        if equals_pos != -1:
            temp_string = lines[1][equals_pos+2:]
            return float(temp_string) / 1000



if __name__=="__main__":
    client = MQTTClient("attic/temperature","192.168.1.15")
    sensor = TemperatureSensor()

    try:
        while True:
            temp = sensor.readTemp()
            print(temp)
            client.publish(str(temp))
            time.sleep(5)


    except KeyboardInterrupt:
        print('Finished.')
