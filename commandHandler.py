from CONSTANTS import *
from logger import *

def decode_command(command):
    instruction = command.split(':')
    if len(instruction) == 2:
        return (instruction[0], instruction[1].split(';'))
    else:
        return (instruction[0], [])

def decode_digital(digital):
    r = []
    r.append((digital>>0)&1)    # SAS BTN 1 STABILITY
    r.append((digital>>1)&1)    # SAS BTN 2 MANOUVER
    r.append((digital>>2)&1)    # SAS BTN 3 PROGRADE
    r.append((digital>>3)&1)    # SAS BTN 4 RETROGRADE
    r.append((digital>>4)&1)    # SAS BTN 5 RADIAL
    r.append((digital>>5)&1)    # SAS BTN 6 ANTIRADIAL
    r.append((digital>>6)&1)    # SAS BTN 7 NORMAL
    r.append((digital>>7)&1)    # SAS BTN 8 ANTINORMAL # TODO: Search for correct order
    r.append((digital>>8)&1)    # STAGE
    r.append((digital>>9)&1)    # LIGHTS
    r.append((digital>>10)&1)   # GEAR
    r.append((digital>>11)&1)   # BRAKES
    r.append((digital>>12)&1)   # SAS SW
    r.append((digital>>13)&1)   # RCS SW
    r.append((digital>>14)&1)   # XY SW
    r.append((digital>>15)&1)   # Z SW
    return r





def handle_command(conn, vessel, command, data):
    lcd_mode = data['lcd_mode']
    lcd_max = data['lcd_max']
    arduino_ready = data['arduino_ready']

    debug('DATA_IN:', data)
    debug('Handling command:', command)
    command = decode_command(command)
    debug('Decoded command:', command)


    instruction = command[0]
    args = command[1]

    if instruction == MODE_UP_IN:
        lcd_mode += 1
        if lcd_mode > lcd_max:
            lcd_mode = 0
    elif instruction == MODE_DOWN_IN:
        lcd_mode -= 1
        if lcd_mode < 0:
            lcd_mode = lcd_max
    elif instruction == DIGITAL_IN:
        digital_data = decode_digital(int(args[0]))
        debug('DIGITAL_DATA:', digital_data)
    elif instruction == ARDUINO_STARTUP:
        arduino_ready = True
    
    return {
        'lcd_mode': lcd_mode,
        'lcd_mode_change': lcd_mode != data['lcd_mode'],
        'arduino_ready': arduino_ready,
    }