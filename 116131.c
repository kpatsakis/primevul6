  Char *write_unsigned_decimal(UInt value, unsigned prefix_size = 0) {
    unsigned num_digits = internal::count_digits(value);
    Char *ptr = get(grow_buffer(prefix_size + num_digits));
    internal::format_decimal(ptr + prefix_size, value, num_digits);
    return ptr;
  }