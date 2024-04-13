  explicit BasicArrayWriter(Char (&array)[SIZE])
    : BasicWriter<Char>(buffer_), buffer_(array, SIZE) {}