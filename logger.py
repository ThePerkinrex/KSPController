from colorama import Fore, Style
import colorama, time, datetime
DEBUG = 0
INFO = 1
WARNING = 2
ERROR = 3

colorama.init()
levelcolors = [Fore.GREEN, Fore.CYAN, Fore.YELLOW+Style.BRIGHT, Fore.RED+Style.BRIGHT]
levels = ['DEBUG', 'INFO', 'WARNING', 'ERROR']


class Logger:
    minimumLevel=1
    def set_minimum_level(self, level):
        self.minimumLevel = level

    def loggger_fmt(SELF, level,msg):
        #print('Logging ', level, minimumLevel, msg)
        if level >= SELF.minimumLevel:
            ts = time.time()
            time_fmt = datetime.datetime.fromtimestamp(ts).strftime('%Y-%m-%d %H:%M:%S')
            msg_to_print = levelcolors[level]+'['+time_fmt+'] '+levels[level]+'\t->\t'+msg+Style.RESET_ALL
            print(msg_to_print)

l = Logger()

def debug(*args):
    msg = ''
    for a in args:
        msg += str(a)
        msg += ' '
    l.loggger_fmt(0, msg)

def info(*args):
    msg = ''
    for a in args:
        msg += str(a)
        msg += ' '
    l.loggger_fmt(1, msg)

def warning(*args):
    msg = ''
    for a in args:
        msg += str(a)
        msg += ' '
    l.loggger_fmt(2, msg)

def warn(*args):
    msg = ''
    for a in args:
        msg += str(a)
        msg += ' '
    l.loggger_fmt(2, msg)

def error(*args):
    msg = ''
    for a in args:
        msg += v
        msg += ' '
    l.loggger_fmt(3, msg)

def err(*args):
    msg = ''
    for a in args:
        msg += str(a)
        msg += ' '
    l.loggger_fmt(3, msg)