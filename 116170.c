  void visit_char(int value) {
    if (spec_.type_ && spec_.type_ != 'c') {
      spec_.flags_ |= CHAR_FLAG;
      writer_.write_int(value, spec_);
      return;
    }
    if (spec_.align_ == ALIGN_NUMERIC || spec_.flags_ != 0)
      FMT_THROW(FormatError("invalid format specifier for char"));
    typedef typename BasicWriter<Char>::CharPtr CharPtr;
    Char fill = internal::CharTraits<Char>::cast(spec_.fill());
    CharPtr out = CharPtr();
    const unsigned CHAR_SIZE = 1;
    if (spec_.width_ > CHAR_SIZE) {
      out = writer_.grow_buffer(spec_.width_);
      if (spec_.align_ == ALIGN_RIGHT) {
        std::uninitialized_fill_n(out, spec_.width_ - CHAR_SIZE, fill);
        out += spec_.width_ - CHAR_SIZE;
      } else if (spec_.align_ == ALIGN_CENTER) {
        out = writer_.fill_padding(out, spec_.width_,
                                   internal::const_check(CHAR_SIZE), fill);
      } else {
        std::uninitialized_fill_n(out + CHAR_SIZE,
                                  spec_.width_ - CHAR_SIZE, fill);
      }
    } else {
      out = writer_.grow_buffer(CHAR_SIZE);
    }
    *out = internal::CharTraits<Char>::cast(value);
  }