"""Start a live exeuction trace for an embedded target using the selected
serial port.

You must provide the map file and SVD information needed for translating trace
values on the command line. You must also specify the serial device. This will
be of the form 'COMn' on Windows or '/dev/ttyn' on Mac or Linux.

Limitations (and areas for future work):
  - The serial port is operated at 921600 with no flow control. There are no
    command-line options to modify this.
  - Raw UINT values on the serial port are not supported. Every value must be
    formatted with either "0x%X\n" or "%u\n" printf format strings.
  - Flash, RAM and peripheral register base addresses are all hard-coded for
    STMicro.
"""
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
    """Read trace buffer values from a serial port back end.
    
    Each value must be in one of the following two formats and terminated with
    a new line character.
      hex format: 0xXXXXXXXX
      decimal format: XXXXXXXXX
    
    Trace buffer data must be the only thing output on this serial port.
    """
    def __init__(self, serial_port: serial.Serial):
        """Initializes the serial port trace reader.
        
        Args:
          serial_port: An already open and configured serial port. This should
                       already be set up with the proper settings for baud
                       rate, stop bits, flow control and so forth.
        """
        self.ser = serial_port

    def read_next(self) -> int:
        """Return the next value from the trace buffer and return it as an
        integer.

        This function reads one line from the serial port. If the serial port's
        RX buffer is empty, it blocks until the next value is available.

        Returns:
          The next value from the trace buffer.
        """
        line = self.ser.read_until()
        line = line.decode(encoding='utf-8')
        value = int(line, 0)
        return value

def live_trace(reader, functions, variables, registers):
    """Start an execution tracer live trace on the selected serial port.
    
    Continuously reads trace values from the serial port and converts them to
    human readable on stdout.

    The user must use ^C to terminate this function.

    Args:
      reader: A SerialPortTraceReader object for retrieving trace buffer
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
    """Start a live exeuction trace for an embedded target using the selected
    serial port.

    See module comment for usage.
    """
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
