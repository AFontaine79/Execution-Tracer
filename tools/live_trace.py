
from parse_map_file import read_gnu_map_file
from parse_svd import get_mcu_register_set

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
        return "Function @ 0x%08X" % func_addr

def get_sfr_name(value, registers):
    sfr_addr = (value & 0xFFFFFFE) + SFR_BASE
    if sfr_addr in registers.keys():
        peripheral_register = registers[sfr_addr]
        return "%s->%s" % (peripheral_register.peripheral_name, peripheral_register.register_name)
    else:
        return "SFR @ 0x%08X" % sfr_addr

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

def trace_sfr(addr_value, reg_value, registers):
    print_indent()
    print("%s = 0x%08X" % (get_sfr_name(addr_value, registers), reg_value))

def live_trace(ser, functions, variables, registers):
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
                line2 = ser.read_until()
                line2 = line2.decode(encoding='utf-8')
                value2 = int(line2, 0)
                trace_sfr(value, value2, registers)
        else:
            print(line)

def main():
    global map_file
    global ser_port

    parser = argparse.ArgumentParser(description='GNU Map file parser')
    parser.add_argument('--map_file', '-m', help='GNU Map file', type=str, required=True)
    parser.add_argument('--serial', '-s', help='Serial device', type=str, required=True)
    parser.add_argument('--svd_file', help='SVD file in XML foramt', type=str, required=False)
    parser.add_argument('--make', help='Vendor (e.g. Atmel or STMicro)', type=str, required=False)
    parser.add_argument('--model', help='Device name (e.g. ATSAMA5D33 or STM32L4x6)', type=str, required=False)
    args = parser.parse_args()

    map_file = args.map_file
    ser_port = args.serial
    svd_file = args.svd_file
    make = args.make
    model = args.model

    functions, variables = read_gnu_map_file(map_file)
    if functions:
        # Can trace functions found here
        pass
    else:
        print("WARNING: No functions found in %s" % map_file)
    if variables:
        # Can trace variables found here
        pass
    else:
        print("WARNING: No variables found in %s" % map_file)
    
    registers = get_mcu_register_set(svd_file, make, model)
    if registers:
        # Can trace registers found here
        pass
    else:
        print("WARNING: No peripheral registers found")

    ser = serial.Serial(port=ser_port, baudrate=921600, rtscts=False)

    live_trace(ser, functions, variables, registers)

class InputError(RuntimeError):
    def __init__(self, e):
        super(InputError, self).__init__(e)

if __name__ == '__main__':
    try:
        main()
    except InputError as e:
        print(e, file=sys.stderr)
        sys.exit(2)
