QPDFWriter::adjustAESStreamLength(size_t& length)
{
    if (this->encrypted && (! this->cur_data_key.empty()) &&
	this->encrypt_use_aes)
    {
	// Stream length will be padded with 1 to 16 bytes to end up
	// as a multiple of 16.  It will also be prepended by 16 bits
	// of random data.
	length += 32 - (length & 0xf);
    }
}