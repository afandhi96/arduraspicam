import serial
import time

class Sensor():
	def __init__(self):
		self.baud = 9600
	
	def baca_usb_com(self):
		list_usb_com = ["/dev/ttyACM0","/dev/ttyACM1","/dev/ttyACM2","/dev/ttyACM3"]
		for j in range(len(list_usb_com)):
			try:
				baca_serial 	= serial.Serial(list_usb_com[j], self.baud, timeout=1)
				usb_com		= list_usb_com[j]
				return usb_com
			except:
				pass
	#tidk di gunakan
	def baca_sensor(self, usb):
		baca_serial = serial.Serial(usb, self.baud)
		data_sensor = str(baca_serial.readline())
		return data_sensor
"""		
if __name__ == '__main__':
	Ss = Sensor()
	print Ss.baca_usb_com()
	baca_usb = Ss.baca_usb_com()
	
	while(1):
		sen = serial.Serial(baca_usb, 9600)
		print sen.readline()
"""	
