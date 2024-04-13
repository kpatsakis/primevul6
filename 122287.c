QPDFWriter::writeStringNoQDF(std::string const& str)
{
    if (! this->qdf_mode)
    {
	writeString(str);
    }
}