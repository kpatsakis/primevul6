QPDFWriter::writeString(std::string const& str)
{
    this->pipeline->write(QUtil::unsigned_char_pointer(str), str.length());
}