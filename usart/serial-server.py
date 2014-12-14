import serial

ser = serial.Serial(4, 2400, timeout=1)

last = ""

while True:
	line = ser.readline()
	try:
		str = line.decode('ascii')
	except Exception:
		str = ""
	if len(str.strip()):
		print(str)