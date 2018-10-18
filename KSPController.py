
import serial
import krpc
from serial.tools.list_ports import comports
from logger import *
import time
import sys
from CONSTANTS import *

l.set_minimum_level(DEBUG)
TIMEOUT = 10

import commandHandler

def invert_trunc(n):
    if n>int(n):
        return int(n)+1
    return int(n)

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
conn = None
while conn is None:
    try:
        conn = krpc.connect('Perc Controller', rpc_port=25565, stream_port=25566)
    except:
        conn = None

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
    debug('Writing:', line)
    ser.write((line+'\n').encode('ascii'))
    ser.flushOutput()

def write_command(commn, *args):
    comm = str(commn)+':'
    for arg in args:
        comm += str(arg)
        if args.index(arg) != args.count-1:
            comm += ';'
    write_line(comm)
    

def close_connection():
    
    #write_line(CONNECTION_END_OUT)
    write_command(CONNECTION_END_OUT)
    ser.close()
    conn.close()

while True:
    if read_line() == ARDUINO_STARTUP:
        info('Controller initialized')
        #write_line('3')
        break

lcd_mode = 0
lcd_max = 1
# 0: Surface altitude
# 1: Speed
lcd_mode_change = False
connected=False
def send_data():
    vessel = conn.space_center.active_vessel
    if not connected:
        #write_line('3:'+str(int(vessel.control.rcs))+';'str(int(vessel.control.sas)))
        write_command(CONNECTION_START_OUT)
        connected = True
    resources = vessel.resources
    lf = '0'
    ox = '0'
    sf = '0'
    mp = '0'
    ec = '0'
    try:
        lf = str(invert_trunc(resources.amount('LiquidFuel')/resources.max('LiquidFuel')*10))
    except ZeroDivisionError:
        pass
    try:
        ox = str(invert_trunc(resources.amount('Oxidizer')/resources.max('Oxidizer')*10))
    except ZeroDivisionError:
        pass
    try:
        sf = str(invert_trunc(resources.amount('SolidFuel')/resources.max('SolidFuel')*10))
    except ZeroDivisionError:
        pass
    try:
        mp = str(invert_trunc(resources.amount('MonoPropellant')/resources.max('MonoPropellant')*10))
    except ZeroDivisionError:
        pass
    try:
        ec = str(invert_trunc(resources.amount('ElectricCharge')/resources.max('ElectricCharge')*10))
    except ZeroDivisionError:
        pass
    #write_line('0:'+lf+';'+ox+';'+sf+';'+mp+';'+ec)
    write_command(FUEL_LEVELS_OUT, lf, ox, sf, mp, ec)
    if lcd_mode_change:
        lcd_mode_change = False
        if lcd_mode == 0:
            # TODO: Make it with m/k/M like the one in KSP
            write_command(LCD_TXT_OUT, 'SURFACE ALTITUDE', vessel.flight().surface_altitude)
            
        if lcd_mode == 1:
            # ? Review needed
            speedmode = vessel.control.speedmode
            reference_frame = vessel.surface_reference_frame
            speedname = 'SURFACE'
            if speedmode == speedmode.orbit:
                reference_frame = vessel.orbital_reference_frame
                speedname = 'ORBIT'
            write_command(LCD_TXT_OUT, speedname + ' SPEED', vessel.flight(reference_frame).surface_altitude)

def handle_command(command):
    vessel = conn.space_center.active_vessel
    commandHandler.handle_command(conn, vessel, command)


while True:
    try:
        if conn.krpc.current_game_scene == conn.krpc.GameScene.flight:
            
            send_data()
            line = read_line()
            if line != '':
                debug(line)
                if line == MODE_UP_IN:
                    lcd_mode_change = True
                    lcd_mode += 1
                    if lcd_mode > lcd_max:
                        lcd_mode = 0
                elif line == MODE_DOWN_IN:
                    lcd_mode_change = True
                    lcd_mode -= 1
                    if lcd_mode < 0:
                        lcd_mode = lcd_max
        else:
            if connected:
                write_command(CONNECTION_END_OUT)
                connected = False

        
    except Exception as e:
        err("Error:", e)
        break
    except KeyboardInterrupt:
        print()
        break
info("Exiting")
close_connection()
info('Exited correctly')