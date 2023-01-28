
import os
import re
import sys
from enum import Enum

class LinkerSection(Enum):
    UNKNOWN = 1
    TEXT = 2
    DATA = 3
    BSS = 4

linker_section_ids = {
    ".text": LinkerSection.TEXT,
    ".data": LinkerSection.DATA,
    ".bss": LinkerSection.BSS
}

def read_gnu_map_file(file_name):
    if not os.path.isfile(file_name):
        print(f"File '{file_name}' not found")
        return (None, None)

    memory_map_section_found = False
    linker_section = LinkerSection.UNKNOWN
    functions = {}
    variables = {}
    with open(file_name, "r") as file:
        for line in file:
            if not memory_map_section_found:
                # There is nothing meaningful to parse before this section
                if line.startswith("Linker script and memory map"):
                    print("Found memory map")
                    memory_map_section_found = True
            else:
                # Each linker section starts with a non-indented lin in the map file
                # This will be .text, .data, .isr_vector, etc.
                if line.startswith('.'):
                    match_results = re.match('^(\.[a-zA-Z_0-9\.]+)\s+', line)
                    if match_results:
                        linker_section_name = match_results.groups()[0]
                        if linker_section_name in linker_section_ids.keys():
                            # We have entered a linker section we care about and will parse
                            print(f"Found linker section {linker_section_name}")
                            linker_section = linker_section_ids[linker_section_name]
                        else:
                            # We have entered something we don't care about
                            # E.g. .ARM.extab or .glue_7 or something
                            linker_section = LinkerSection.UNKNOWN
                    else:
                        print("Fix linker section pattern matching regex")
                        sys.exit(1)
                else:
                    # For this section to work properly, we need the -ffunction-sections
                    # and -fdata-sections compile flags enabled. Also, this will not be
                    # able to see static variables. Suggested workaround for tracing
                    # static variables is to not declare them static for debug builds
                    # where you want them traced.
                    if linker_section == LinkerSection.TEXT:
                        match_results = re.match('^ {16}0x([0-9A-Fa-f]+) {16}(\S+)', line)
                        if match_results:
                            # Function name and address found
                            address = int(match_results.groups()[0], 16)
                            functions[address] = match_results.groups()[1]
                    elif linker_section == LinkerSection.DATA or linker_section == LinkerSection.BSS:
                        match_results = re.match('^ {16}0x([0-9A-Fa-f]+) {16}(\S+)', line)
                        if match_results:
                            # Variable name and address found
                            address = int(match_results.groups()[0], 16)
                            variables[address] = match_results.groups()[1]
    
    return (functions, variables)
