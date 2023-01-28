
from parse_map_file import read_gnu_map_file
import argparse
import serial
import sys

FLASH_BASE = 0x08000000
RAM_BASE   = 0x20000000
SFR_BASE   = 0x40000000

indent_level = 0

def inc_indent():
    global indent_level
    indent_level = indent_level + 1

def dec_indent():
    global indent_level
    if indent_level > 0:
        indent_level = indent_level - 1

def print_indent():
    global indent_level
    for i in range(0, indent_level):
        print("  ", end="")

def reset_indent():
    global indent_level
    indent_level = 0

def get_func_name(value, functions):
    func_addr = (value & 0xFFFFFFE) + FLASH_BASE
    if func_addr in functions.keys():
        return functions[func_addr]
    else:
        return "Unknown func"

def trace_version(value):
    ver_char = (value >> 16) & 0xFF
    ver_major = (value >> 8) & 0xFF
    ver_minor = (value >> 0) & 0xFF
    reset_indent()
    print("Tracer version %c%d.%d" % (ver_char, ver_major, ver_minor))

def trace_reset(value):
    print_indent()
    print("Processor reset: 0x%02X" % value)

def trace_func_entry(value, functions):
    print_indent()
    print("Enter %s" % get_func_name(value, functions))
    # Increment indent after Enter statement
    # This is like an opening brace
    inc_indent()

def trace_func_exit(value, functions):
    # Decrement indent before Exit statement
    # This is like a closing brace
    dec_indent()
    print_indent()
    print("Exit %s" % get_func_name(value, functions))

def trace_file_and_line(value):
    pass

def trace_variable(value):
    pass

def trace_sfr(value):
    pass

def live_trace(ser, functions, variables):
    while(True):
        line = ser.read_until()
        line = line.decode(encoding='utf-8')
        if line.startswith('0x'):
            value = int(line, 0)
            idcode = (value >> 28) & 0xF
            if idcode == 1:
                trace_version(value)
            elif idcode == 2:
                trace_reset(value)
            elif idcode == 3:
                trace_func_entry(value, functions)
            elif idcode == 4:
                trace_func_exit(value, functions)
            elif idcode == 5:
                pass
            elif idcode == 6:
                pass
            elif idcode == 7:
                pass
        else:
            print(line)

def main():
    global map_file
    global ser_port

    parser = argparse.ArgumentParser(description='GNU Map file parser')
    parser.add_argument('--file', '-f', help='GNU Map file', type=str, required=True)
    parser.add_argument('--port', '-p', help='Serial port', type=str, required=True)
    args = parser.parse_args()

    map_file = args.file
    ser_port = args.port

    functions, variables = read_gnu_map_file(map_file)
    if functions:
        # Can trace functions found here
        pass
    else:
        print("No functions found in %s" % map_file)
    if variables:
        # Can trace variables found here
        pass
    else:
        print("No variables found in %s" % map_file)

    ser = serial.Serial(port=ser_port, baudrate=921600, rtscts=False)

    live_trace(ser, functions, variables)

class InputError(RuntimeError):
    def __init__(self, e):
        super(InputError, self).__init__(e)

if __name__ == '__main__':
    try:
        main()
    except InputError as e:
        print(e, file=sys.stderr)
        sys.exit(2)
