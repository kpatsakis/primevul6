QPDFWriter::setStreamDataMode(qpdf_stream_data_e mode)
{
    this->stream_data_mode_set = true;
    this->stream_data_mode = mode;
}