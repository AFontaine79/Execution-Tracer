
from parse_map_file import read_gnu_map_file
from parse_svd import get_mcu_register_set
from exec_trace_parser import ExecTraceParser, TraceReaderInterface

import argparse
import io
import sys

FLASH_BASE = 0x08000000
RAM_BASE   = 0x20000000
SFR_BASE   = 0x40000000

# TODO: Figure out how we can properly iterate the text file
class TextFileTraceReader(TraceReaderInterface):
    def __init__(self, log_file: io.StringIO):
        self.log_file = log_file

    def read_next(self) -> int:
        line = self.log_file.readline()
        if len(line) > 0:
            value = int(line, 0)
        else:
            value = -1
        return value

def live_trace(reader, functions, variables, registers):
    tracer = ExecTraceParser(functions, variables, registers)
    tracer.set_flash_base(FLASH_BASE)
    tracer.set_ram_base(RAM_BASE)
    tracer.set_sfr_base(SFR_BASE)
    tracer.read_and_trace_all(reader)

def main():
    global ser

    parser = argparse.ArgumentParser(description='GNU Map file parser')
    parser.add_argument('--map_file', '-m', help='GNU Map file', type=str, required=True)
    parser.add_argument('--file', '-f', help='Log file', type=str, required=True)
    parser.add_argument('--svd_file', help='SVD file in XML foramt', type=str, required=False)
    parser.add_argument('--make', help='Vendor (e.g. Atmel or STMicro)', type=str, required=False)
    parser.add_argument('--model', help='Device name (e.g. ATSAMA5D33 or STM32L4x6)', type=str, required=False)
    args = parser.parse_args()

    map_file = args.map_file
    log_file_name = args.file
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

    with open(log_file_name) as log_file:
        reader = TextFileTraceReader(log_file)
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
