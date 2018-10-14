
import serial
import krpc
from serial.tools.list_ports import comports
from logger import *
import time
import sys

l.set_minimum_level(DEBUG)
TIMEOUT = 10

exit_serial_port_find = False
attempts = 0
warned = False
while not exit_serial_port_find:
    if attempts > 25 and not warned:
        warn('Please connect the controller, waiting 10 seconds to do so')
        time.sleep(5)
        for i in range(5):
            warn(str(5-i), 'seconds')
            time.sleep(1)
        warn('I hope you connected the controller, because if not I will error out')
        warned = True
    if attempts > 50:
        error('Max attempts surpassed, exiting')
        exit()
    for port in comports():
        if port.device == '/dev/cu.usbmodem14201':
            exit_serial_port_find = True
            break
        attempts += 1
    

info('Serial port connected')
ser = serial.Serial('/dev/cu.usbmodem14201', 9600, timeout=1)
info('Serial connection established')
conn = krpc.connect('Perc Controller', rpc_port=25565, stream_port=25566)
info('Connection established with KSP')

def read_line():
    string = ''
    attempts = 0
    while ser.in_waiting>0:
        char = ser.read(1).decode('ascii')
        
        if char == '\r':
            ser.read(1)
            break
        else:
            string += char
            attempts += 1
    
    return string

def write_line(line):
    ser.write((line+'\n').encode('ascii'))
    ser.flushOutput()

def close_connection():
    
    write_line('4')
    ser.close()
    conn.close()

while True:
    if read_line() == '4':
        info('Controller initialized')
        write_line('3')
        break

#idx = 0
while True:
    #debug('Looping')
    try:
        #write_line('1:Hello;'+str(idx))
        if conn.krpc.current_game_scene == conn.krpc.GameScene.flight:
            vessel = conn.space_center.active_vessel
            resources = vessel.resources
            debug('SolidFuel:', resources.amount('SolidFuel'))
            line = read_line()
            if line != '':
                debug(line)
        #idx += 1
        #time.sleep(0.5)
    except Exception as e:
        err("Error:", e)
        break
    except KeyboardInterrupt:
        print()
        break
info("Exiting")
close_connection()
info('Exited correctly')