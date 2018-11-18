
from commandHandler import handle_command
from CONSTANTS import *
from logger import set_minimum_level, DEBUG, info, debug
set_minimum_level(DEBUG)
from perctest.__main__ import TestCase

class Test_CommandHandler(TestCase):
    def test_digital_in(self):
        d = {'lcd_mode': 0, 'lcd_max': 1, 'arduino_ready': True}
        o = {'lcd_mode': 0, 'lcd_mode_change': False, 'arduino_ready': True}
        info('TESTING','Handle command for DIGITAL_IN:5, or 0:5, the 5 decoded digital should start with 101...')
        to = handle_command(0,0,DIGITAL_IN+':5',d)
        info('TESTING OUTPUT:',to)
        self.assertEquals(to,o)
        print('---------------------------------------------')

    def test_analog_in(self):
        d = {'lcd_mode': 0, 'lcd_max': 1, 'arduino_ready': False}
        o = {'lcd_mode': 0, 'lcd_mode_change': False, 'arduino_ready': False}
        info('TESTING Handle command for ANALOG_IN:5;32;4;45;42 , or 1:5;32;4;45;42')
        to = handle_command(0,0,ANALOG_IN+':5',d)
        info('TESTING OUTPUT:',to)
        self.assertEquals(to,o)
        print('---------------------------------------------')
    
    def test_mode_up_in(self):
        d = {'lcd_mode': 0, 'lcd_max': 2, 'arduino_ready': True}
        o = {'lcd_mode': 1, 'lcd_mode_change': True, 'arduino_ready': True}
        info('TESTING Handle command for MODE_UP')
        to = handle_command(0,0,MODE_UP_IN,d)
        info('TESTING OUTPUT:',to)
        self.assertEquals(to,o)
        print('---------------------------------------------')
    
    def test_mode_down_in(self):
        d = {'lcd_mode': 0, 'lcd_max': 2, 'arduino_ready': True}
        o = {'lcd_mode': 2, 'lcd_mode_change': True, 'arduino_ready': True}
        info('TESTING Handle command for MODE_DOWN')
        to = handle_command(0,0,MODE_DOWN_IN,d)
        info('TESTING OUTPUT:',to)
        self.assertEquals(to,o)
        print('---------------------------------------------')
    
    def test_arduino_startup_in(self):
        d = {'lcd_mode': 0, 'lcd_max': 2, 'arduino_ready': False}
        o = {'lcd_mode': 0, 'lcd_mode_change': False, 'arduino_ready': True}
        info('TESTING Handle command for ARDUINO_STARTUP')
        to = handle_command(0,0,ARDUINO_STARTUP,d)
        info('TESTING OUTPUT:',to)
        self.assertEquals(to,o)
        print('---------------------------------------------')