import mysql.connector
from mysql.connector import Error
import paho.mqtt.client as mqtt
import paho.mqtt.publish as publish
from datetime import date
today = date.today()
print(today)

def on_connect(client, userdata, flags, rc):
	print("Connected with result code "+str(rc))
	
def on_message(client, userdata, msg):
	print(msg.topic+" "+str(msg.payload))

client = mqtt.Client()

client.connect("11.11.11.111", 1883, 60)

try:
	connection = mysql.connector.connect(host='localhost',database='kredit',user='root',password='venuslovemechain')
	sql_select_Query = "select * from motor"
	cursor = connection.cursor()
	cursor.execute(sql_select_Query)
	records = cursor.fetchall()
	
	
	for row in records:
		id = row[0]
		tanggal = row[2]
		
		if tanggal == today:
			print(id)
			print(tanggal)
			idstring = str(id)
			publish.single(idstring, "0", hostname="11.11.11.111")
			
	
except Error as e:
	print("Error reading data from MySQL table", e)