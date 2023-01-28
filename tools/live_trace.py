
from parse_map_file import read_gnu_map_file
from parse_svd import get_mcu_register_set
from exec_trace_parser import ExecTraceParser, TraceReaderInterface

import argparse
import serial
import sys

FLASH_BASE = 0x08000000
RAM_BASE   = 0x20000000
SFR_BASE   = 0x40000000

class SerialPortTraceReader(TraceReaderInterface):
    def __init__(self, serial_port: serial.Serial):
        self.ser = serial_port

    def read_next(self) -> int:
        line = self.ser.read_until()
        line = line.decode(encoding='utf-8')
        value = int(line, 0)
        return value

def live_trace(reader, functions, variables, registers):
    tracer = ExecTraceParser(functions, variables, registers)
    tracer.set_flash_base(FLASH_BASE)
    tracer.set_ram_base(RAM_BASE)
    tracer.set_sfr_base(SFR_BASE)
    while(True):
        tracer.read_and_trace_next(reader)

def main():
    global ser

    parser = argparse.ArgumentParser(description='GNU Map file parser')
    parser.add_argument('--map_file', '-m', help='GNU Map file', type=str, required=True)
    parser.add_argument('--serial', '-s', help='Serial device', type=str, required=True)
    parser.add_argument('--svd_file', help='SVD file in XML foramt', type=str, required=False)
    parser.add_argument('--make', help='Vendor (e.g. Atmel or STMicro)', type=str, required=False)
    parser.add_argument('--model', help='Device name (e.g. ATSAMA5D33 or STM32L4x6)', type=str, required=False)
    args = parser.parse_args()

    map_file = args.map_file
    ser_port_name = args.serial
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

    reader = SerialPortTraceReader(serial.Serial(port=ser_port_name, baudrate=921600, rtscts=False))

    live_trace(reader, functions, variables, registers)

class InputError(RuntimeError):
    def __init__(self, e):
        super(InputError, self).__init__(e)

if __name__ == '__main__':
    try:
        main()
    except InputError as e:
        print(e, file=sys.stderr)
        sys.exit(2)
