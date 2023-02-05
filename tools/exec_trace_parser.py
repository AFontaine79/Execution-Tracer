class TraceReaderInterface:
    """Interface expected by ExecTraceParser for returning trace buffer values.

    Users of this module must extend this class to provide trace buffer values
    to ExecTraceParser. How values are retrieved from an execution tracer
    backend is implementation specific.
    """

    """Value to return after the last value has been read."""
    END_OF_TRACE_BUFFER = -1

    def read_next(self) -> int:
        """Return the next value from the trace buffer.

        This function is allowed to block indefinitely. This is to allow for
        use in live trace scenarios where the trace reader will be continually
        waiting for new values to arrive.

        Returns:
          The next value from the trace buffer, or END_OF_TRACE_BUFFER.
        """
        pass

class ExecTraceParser:
    """Translates trace buffer values into human readable text."""
    def __init__(self, functions, variables, registers):
        """Initialize the parser with all of the look-up dictionaries.

        Args:
          functions: Dictionary that maps MCU addresses to function names.
          variables: Dictionary that maps MCU addresses to variable names.
          registers: Dictionary that maps MCU addresses to
                     parse_svd.PeripheralRegister objects.
          """
        self.functions = functions
        self.variables = variables
        self.registers = registers
        # The parser needs to know the MCU's base addresses for flash, RAM and
        # peripheral register regions in order to use the look-up dictionaries
        # properly.
        # These values can be assigned explicitly from separate functions.
        self.FLASH_BASE = 0
        self.RAM_BASE = 0
        self.SFR_BASE = 0
        # The indent level is used to keep of nested function calls.
        self.indent_level = 0

    def set_flash_base(self, flash_base):
        """Set the base address for the MCU's flash region."""
        self.FLASH_BASE = flash_base

    def set_ram_base(self, ram_base):
        """Set the base address for the MCU's RAM region."""

        self.RAM_BASE = ram_base

    def set_sfr_base(self, sfr_base):
        """Set the base address for the MCU's peripherals region."""
        self.SFR_BASE = sfr_base

    def inc_indent(self):
        """Increments the indent level for trace output."""
        self.indent_level = self.indent_level + 1

    def dec_indent(self):
        """Decreases the indent level for trace output."""
        if self.indent_level > 0:
            self.indent_level = self.indent_level - 1

    def print_indent(self):
        """Outputs the indent for a single trace line."""
        for i in range(0, self.indent_level):
            print("  ", end="")

    def reset_indent(self):
        """Resets the indent level.

        This is necessary if the traces continue through a fault situation and
        into a restart of the MCU. The nesting must be reset to 0. The parser
        knows of this condition due to the trace parser version trace, which is
        part of (re)initialization of the execution tracer on startup.
        """
        self.indent_level = 0

    def get_func_name(self, value):
        """Translate a raw trace value into its corresponding function name.

        Args:
          value: A raw function entry or function exit trace buffer value.

        Returns:
          If lookup succeeds: the function's name.
          If lookup fails: the string "Function @ <address>"
        """
        func_addr = (value & 0xFFFFFFE) + self.FLASH_BASE
        if func_addr in self.functions.keys():
            return self.functions[func_addr]
        else:
            return "Function @ 0x%08X" % func_addr

    def get_var_name(self, value):
        """Translate a raw trace value into its corresponding variable name.

        Args:
          value: A raw variable trace buffer value.

        Returns:
          If lookup succeeds: the variable's name.
          If lookup fails: the string "Variable @ <address>"
        """
        var_addr = (value & 0xFFFFFFE) + self.RAM_BASE
        if var_addr in self.variables.keys():
            return self.variables[var_addr]
        else:
            return "Variable @ 0x%08X" % var_addr

    def get_sfr_name(self, value):
        """Translate a raw trace value into its corresponding register name.

        Args:
          value: A raw SFR trace buffer value.

        Returns:
          If lookup succeeds: the registers's name in the format of
            Peripheral->Register (i.e. how it might look in the code)
            Examples: "USART1->CR1"
                      "TIM2->CCR1"
                      "RCC->PLLCFGR"
          If lookup fails: the string "SFR @ <address>"
        """
        sfr_addr = (value & 0xFFFFFFE) + self.SFR_BASE
        if sfr_addr in self.registers.keys():
            peripheral_register = self.registers[sfr_addr]
            return "%s->%s" % (peripheral_register.peripheral_name, peripheral_register.register_name)
        else:
            return "SFR @ 0x%08X" % sfr_addr

    def trace_version(self, value):
        """Translate a TRACE_ExecTracerVersion() trace to human readable output."""
        ver_char = (value >> 16) & 0xFF
        ver_major = (value >> 8) & 0xFF
        ver_minor = (value >> 0) & 0xFF
        self.reset_indent()
        print("Tracer version %c%d.%d" % (ver_char, ver_major, ver_minor))

    def trace_reset(self, value):
        """Translate a TRACE_ProcessorReset() trace to human readable output."""
        self.print_indent()
        print("Processor reset: 0x%02X" % value)

    def trace_func_entry(self, value):
        """Translate a TRACE_FunctionEntry() trace to human readable output."""
        self.print_indent()
        print("Enter %s" % self.get_func_name(value))
        # Increment indent after Enter statement
        # This is like an opening brace
        self.inc_indent()

    def trace_func_exit(self, value):
        """Translate a TRACE_FunctionExit() trace to human readable output."""
        # Decrement indent before Exit statement
        # This is like a closing brace
        self.dec_indent()
        self.print_indent()
        print("Exit %s" % self.get_func_name(value))

    def trace_file_and_line(self, value):
        """Translate a TRACE_Line() trace to human readable output."""
        module_num = (value >> 16) & 0xFFF
        line_num = (value >> 0) & 0xFFFF
        self.print_indent()
        print("Module: %u, Line: %u" % (module_num, line_num))

    def trace_variable(self, addr_value, var_value):
        """Translate a TRACE_VariableValue() trace to human readable output."""
        self.print_indent()
        print("%s = %d" % (self.get_var_name(addr_value), var_value))

    def trace_sfr(self, addr_value, reg_value):
        """Translate a TRACE_SFRValue() trace to human readable output."""
        self.print_indent()
        print("%s = 0x%08X" % (self.get_sfr_name(addr_value), reg_value))

    def read_and_trace_next(self, trace_reader: TraceReaderInterface):
        """Read the next value from the trace reader and translate it to human
        readable output.

        Output is written to stdout.

        Args:
          trace_reader: A concrete implementation of TraceReaderInterface.

        Returns:
          True if there are more values to read.
          False if the end of the buffer has been reached.
        """
        value = trace_reader.read_next()

        if value == TraceReaderInterface.END_OF_TRACE_BUFFER:
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
        """Read and trace values from the trace reader until end of buffer is
        reached.

        For TraceReaderInterface implementations that do not signal end of
        buffer (e.g. live trace scenarios), it will be necessary to terminate
        execution using ^C.

        Args:
          trace_reader: A concrete implementation of TraceReaderInterface.
        """
        while(self.read_and_trace_next(trace_reader)):
            pass
