"""Parse and display the contents of an execution trace log saved to a text
file.

You must provide the map file and SVD information needed for translating trace
values on the command line. You must also provide the path to the log file
where traces are saved.

The log file should likely be the recording from a terminal interface (such as
RTT or serial port) to an execution tracer back-end.

Limitations (and areas for future work):
  - Raw trace buffer data is not supported. Every value must be
    formatted with either "0x%X\n" or "%u\n" printf format strings.
  - Flash, RAM and peripheral register base addresses are all hard-coded for
    STMicro.
"""
from parse_map_file import read_gnu_map_file
from parse_svd import get_mcu_register_set
from exec_trace_parser import ExecTraceParser, TraceReaderInterface

import argparse
import io
import sys

FLASH_BASE = 0x08000000
RAM_BASE   = 0x20000000
SFR_BASE   = 0x40000000

class TextFileTraceReader(TraceReaderInterface):
    """Read trace buffer values saved to a log file in text format.
    
    Each value must be in one of the following two formats and terminated with
    a new line character.
      hex format: 0xXXXXXXXX
      decimal format: XXXXXXXXX
    
    The log file must not contain anything other than trace buffer data.
    """
    def __init__(self, log_file: io.StringIO):
        """Initializes the log file trace reader.
        
        Args:
          log_file: A text stream reader object returned by open().
        """
        self.log_file = log_file

    def read_next(self) -> int:
        """Read the next value from the log file and return it as an integer.
        
        Returns:
          The next value from the log file or
          TraceReaderInterface.END_OF_TRACE_BUFFER if there are no more values.
        """
        line = self.log_file.readline()
        if len(line) > 0:
            value = int(line, 0)
        else:
            value = TraceReaderInterface.END_OF_TRACE_BUFFER
        return value

def live_trace(reader, functions, variables, registers):
    """Parse all values from the log file and output to stdout.
    
    Iterates over the entire log file, translating all trace values to human
    readable format and outputting the results to stdout.

    The user must use ^C to terminate this function.

    Args:
      reader: A TextFileTraceReader object for retrieving trace buffer
              values.
      functions: Dictionary that maps MCU addresses to function names.
      variables: Dictionary that maps MCU addresses to variable names.
      registers: Dictionary that maps MCU addresses to
                 parse_svd.PeripheralRegister objects.
  
    """
    tracer = ExecTraceParser(functions, variables, registers)
    tracer.set_flash_base(FLASH_BASE)
    tracer.set_ram_base(RAM_BASE)
    tracer.set_sfr_base(SFR_BASE)
    tracer.read_and_trace_all(reader)

def main():
    """Parse a log file saved from a back-end terminal and output the results
    to stdout.
    
    See module comment for usage.
    """
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
    """Boilerplate code for using this file directly from the command line."""
    def __init__(self, e):
        super(InputError, self).__init__(e)

if __name__ == '__main__':
    """Boilerplate code for using this file directly from the command line."""
    try:
        main()
    except InputError as e:
        print(e, file=sys.stderr)
        sys.exit(2)
