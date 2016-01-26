#!/usr/bin/python
# filename: datademo.py 
# a simple script to pull some data from a MySQL table

import MFRC522
import signal
import MySQLdb
import RPi.GPIO as GPIO ## Import GPIO library
import time ## Import 'time' library. Allows us to use 'sleep'

# Setup PIN to connect door relay to and LED to show door open
GPIO.setmode(GPIO.BOARD) # Use board pin numbering
GPIO.setup(7, GPIO.OUT) # Setup GPIO Pin 7 to OUT
GPIO.output(7,True)# set pin 7 off

db = MySQLdb.connect(host="127.0.0.1", user="root", passwd="Hello123", db="SLOROBOTICSRFID")
#create a cursor for the select
rs = db.cursor()

continue_reading = True
MIFAREReader = MFRC522.MFRC522()

def end_read(signal, frame):
  global continue_reading
  continue_reading = False
  print "Ctrl+C captured, ending read."
  MIFAREReader.GPIO_CLEEN()

signal.signal(signal.SIGINT, end_read)

while continue_reading:
  (status,TagType) = MIFAREReader.MFRC522_Request(MIFAREReader.PICC_REQIDL)
  if status == MIFAREReader.MI_OK:
    print "Card detected"
  (status,backData) = MIFAREReader.MFRC522_Anticoll()
  if status == MIFAREReader.MI_OK:
	print "Card ID: "+str(backData[0])+str(backData[1])+str(backData[2])+str(backData[3])+str(backData[4])
 	results  = rs.execute("SELECT fname,lname,active FROM users WHERE cardid="+str(backData[0])+str(backData[1])+str(backData[2])+str(backData[3])+str(backData[4]))
	if results:
		# loop to iterate
		for row in rs.fetchall() :
	      #data from rows
			firstname = str(row[0])
			lastname = str(row[1])
			active = str(row[2])
			print "Name: " + firstname + " " + lastname
			if active=='y':
				print "Memeber account Active: YES"
				GPIO.output(7,False) ## Turn on GPIO pin 7 - open door latch
				time.sleep(1)## Wait
				GPIO.output(7,True)## Switch off pin 7  - lock door latch
			else:
				print "Memeber account Active: NO"

		print " "
		db.commit()

	else:
		db.commit()
		print "No Records/Card match in DB"
		print " "
