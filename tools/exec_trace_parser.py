class TraceReaderInterface:
    def read_next(self) -> int:
        """Return the next value from the trace buffer.

        The value must be returned as an integer regardless
        of its intermediate format. This function may block
        if waiting for new values.
        """
        pass

class ExecTraceParser:
    def __init__(self, functions, variables, registers):
        self.functions = functions
        self.variables = variables
        self.registers = registers
        self.FLASH_BASE = 0
        self.RAM_BASE = 0
        self.SFR_BASE = 0
        self.indent_level = 0

    def set_flash_base(self, flash_base):
        self.FLASH_BASE = flash_base

    def set_ram_base(self, ram_base):
        self.RAM_BASE = ram_base

    def set_sfr_base(self, sfr_base):
        self.SFR_BASE = sfr_base

    def inc_indent(self):
        self.indent_level = self.indent_level + 1

    def dec_indent(self):
        if self.indent_level > 0:
            self.indent_level = self.indent_level - 1

    def print_indent(self):
        for i in range(0, self.indent_level):
            print("  ", end="")

    def reset_indent(self):
        self.indent_level = 0

    def get_func_name(self, value):
        func_addr = (value & 0xFFFFFFE) + self.FLASH_BASE
        if func_addr in self.functions.keys():
            return self.functions[func_addr]
        else:
            return "Function @ 0x%08X" % func_addr

    def get_var_name(self, value):
        var_addr = (value & 0xFFFFFFE) + self.RAM_BASE
        if var_addr in self.variables.keys():
            return self.variables[var_addr]
        else:
            return "Variable @ 0x%08X" % var_addr

    def get_sfr_name(self, value):
        sfr_addr = (value & 0xFFFFFFE) + self.SFR_BASE
        if sfr_addr in self.registers.keys():
            peripheral_register = self.registers[sfr_addr]
            return "%s->%s" % (peripheral_register.peripheral_name, peripheral_register.register_name)
        else:
            return "SFR @ 0x%08X" % sfr_addr

    def trace_version(self, value):
        ver_char = (value >> 16) & 0xFF
        ver_major = (value >> 8) & 0xFF
        ver_minor = (value >> 0) & 0xFF
        self.reset_indent()
        print("Tracer version %c%d.%d" % (ver_char, ver_major, ver_minor))

    def trace_reset(self, value):
        self.print_indent()
        print("Processor reset: 0x%02X" % value)

    def trace_func_entry(self, value):
        self.print_indent()
        print("Enter %s" % self.get_func_name(value))
        # Increment indent after Enter statement
        # This is like an opening brace
        self.inc_indent()

    def trace_func_exit(self, value):
        # Decrement indent before Exit statement
        # This is like a closing brace
        self.dec_indent()
        self.print_indent()
        print("Exit %s" % self.get_func_name(value))

    def trace_file_and_line(self, value):
        module_num = (value >> 16) & 0xFFF
        line_num = (value >> 0) & 0xFFFF
        self.print_indent()
        print("Module: %u, Line: %u" % (module_num, line_num))

    def trace_variable(self, addr_value, var_value):
        self.print_indent()
        print("%s = %d" % (self.get_var_name(addr_value), var_value))

    def trace_sfr(self, addr_value, reg_value):
        self.print_indent()
        print("%s = 0x%08X" % (self.get_sfr_name(addr_value), reg_value))

    def read_and_trace_next(self, trace_reader: TraceReaderInterface):
        value = trace_reader.read_next()

        if value == -1:
            return False

        idcode = (value >> 28) & 0xF
        if idcode == 1:
            self.trace_version(value)
        elif idcode == 2:
            self.trace_reset(value)
        elif idcode == 3:
            self.trace_func_entry(value)
        elif idcode == 4:
            self.trace_func_exit(value)
        elif idcode == 5:
            self.trace_file_and_line(value)
        elif idcode == 6:
            value2 = trace_reader.read_next()
            self.trace_variable(value, value2)
        elif idcode == 7:
            value2 = trace_reader.read_next()
            self.trace_sfr(value, value2)

        return True

    def read_and_trace_all(self, trace_reader: TraceReaderInterface):
        while(self.read_and_trace_next(trace_reader)):
            pass
