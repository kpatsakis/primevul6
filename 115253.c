int32_t ByteArray::CopyTo(OutputStream* os) {
    return CopyTo(os, 0, Length());
}