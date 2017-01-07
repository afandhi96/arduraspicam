import io
import time
import picamera
import threading

class Picam(object):
	thread		= None
	frame		= None
	last_take	= 0

	def initialize(self):
		if Picam.thread is None:
			Picam.thread = threading.Thread(target = self._thread)
			Picam.thread.start()

			while self.frame is None:
				time.sleep(0)

	def get_frame(self):
		Picam.last_take = time.time()
		self.initialize()
		return self.frame
	
	@classmethod
	def _thread(cls):
		with picamera.PiCamera() as cam:
			
			cam.resolution	= (1000, 550)
			cam.hflip	= True
			cam.vflip	= True

			cam.start_preview()
			time.sleep(2)

			stream = io.BytesIO()
			for j in cam.capture_continuous(stream, 'jpeg', use_video_port = True):
				stream.seek(0)
				cls.frame = stream.read()
	
				stream.seek(0)
				stream.truncate()

				if time.time() - cls.last_take > 10:
					break
		cls.thread = None
				
