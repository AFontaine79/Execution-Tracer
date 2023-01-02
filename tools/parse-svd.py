
import argparse
import os
import re
import sys
from cmsis_svd.parser import SVDParser

class PeripheralRegister:
    def __init__(self):
        self.peripheral_name = ""
        self.register_name = ""

def get_mcu_register_set_for_device(device):
    registers = {}
    for peripheral in device.peripherals:
        peripheral_name = peripheral.name
        peripheral_address = peripheral.base_address
        for register in peripheral.registers:
            register_name = register.name
            register_offset = register.address_offset
            register_address = peripheral_address + register_offset
            registers[register_address] = PeripheralRegister()
            registers[register_address].peripheral_name = peripheral_name
            registers[register_address].register_name = register_name
    return registers

def get_mcu_register_set_for_packaged_svd(make, model):
    try:
        parser = SVDParser.for_packaged_svd(make, model + '.svd')
        device = parser.get_device()
        return get_mcu_register_set_for_device(device)
    except FileNotFoundError as e:
        print(f"{model} by {make}: Device not found")
        return None

def get_mcu_register_set_from_xml_file(file_name):
    if not os.path.isfile(file_name):
        print(f"File '{file_name}' not found")
        return None

    try:
        parser = SVDParser.for_xml_file(file_name)
        device = parser.get_device()
        return get_mcu_register_set_for_device(device)
    except Exception as e:
        print(e, file=sys.stderr)
        return None

def get_mcu_register_set(svd_file = None, make = None, model = None):
    if not make and not model and not svd_file:
        print("WARNING: No arguments for SVD selection")
        return None

    if (svd_file and (make or model)) or (make and not model) or (model and not make):
        print("WARNING: Invalid arguments combination for SVD selection")
        print("Provide MAKE and MODEL to use a packaged svd file")
        print("Provide SVD_FILE to use an external svd file")
        print("Do not use both at the same time; They are mutually exclusive")
        return None

    if make and model:
        return get_mcu_register_set_for_packaged_svd(make, model)
    else:
        return get_mcu_register_set_from_xml_file(svd_file)

def main():
    parser = argparse.ArgumentParser(description='SVD Parser')
    parser.add_argument('--svd_file', help='SVD file in XML foramt', type=str, required=False)
    parser.add_argument('--make', help='Vendor (e.g. Atmel or STMicro)', type=str, required=False)
    parser.add_argument('--model', help='Device name (e.g. ATSAMA5D33 or STM32L4x6)', type=str, required=False)
    args = parser.parse_args()

    svd_file = args.svd_file
    make = args.make
    model = args.model

    registers = get_mcu_register_set(svd_file, make, model)

    if registers:
        for key in registers.keys():
            peripheral_register = registers[key]
            print("%s->%s @ 0x%08X" % (peripheral_register.peripheral_name, peripheral_register.register_name, key))

class InputError(RuntimeError):
    def __init__(self, e):
        super(InputError, self).__init__(e)

if __name__ == '__main__':
    try:
        main()
    except InputError as e:
        print(e, file=sys.stderr)
        sys.exit(2)
