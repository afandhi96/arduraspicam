#!/usr/bin/env python
from flask import Flask, render_template, Response, jsonify
from picam import Picam
from sensor import Sensor
import serial

main = Flask(__name__)

#camera
def buat_frame(camera):

        while True:
                frame = camera.get_frame()
                yield (b'--frame\r\n'b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n')

#sensor

def eventStream():
	while True:
		try:
			sens	= Sensor()
			sen	= serial.Serial(sens.baca_usb_com(), 9600)
			temp	= sen.readline()
			yield "data:%s \n\n" %temp 
			
		except:
			break

#routing ke file template/index.html
@main.route('/')
def index():
	return render_template('index.html')

#render data video
@main.route('/render_video')
def render_video():
	return Response(buat_frame(Picam()), mimetype = 'multipart/x-mixed-replace; boundary = frame')

#render data sensor
@main.route('/stream')
def stream():
	return Response(eventStream(), mimetype = 'text/event-stream')

#-----------------
if __name__=='__main__':
	main.run( host = '0.0.0.0', debug = True, threaded = True)
