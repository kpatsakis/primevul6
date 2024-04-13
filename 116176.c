  BasicArrayWriter(Char *array, std::size_t size)
    : BasicWriter<Char>(buffer_), buffer_(array, size) {}