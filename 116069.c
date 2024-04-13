  CharPtr prepare_int_buffer(unsigned num_digits,
      const EmptySpec &, const char *prefix, unsigned prefix_size) {
    unsigned size = prefix_size + num_digits;
    CharPtr p = grow_buffer(size);
    std::uninitialized_copy(prefix, prefix + prefix_size, p);
    return p + size - 1;
  }