QPDFWriter::getBuffer()
{
    Buffer* result = this->output_buffer;
    this->output_buffer = 0;
    return result;
}