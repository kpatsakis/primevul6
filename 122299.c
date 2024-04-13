QPDFWriter::writeBinary(unsigned long long val, unsigned int bytes)
{
    if (bytes > sizeof(unsigned long long))
    {
        throw std::logic_error(
            "QPDFWriter::writeBinary called with too many bytes");
    }
    unsigned char data[sizeof(unsigned long long)];
    for (unsigned int i = 0; i < bytes; ++i)
    {
	data[bytes - i - 1] = static_cast<unsigned char>(val & 0xff);
	val >>= 8;
    }
    this->pipeline->write(data, bytes);
}