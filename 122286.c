QPDFWriter::setExtraHeaderText(std::string const& text)
{
    this->extra_header_text = text;
    if ((this->extra_header_text.length() > 0) &&
        (*(this->extra_header_text.rbegin()) != '\n'))
    {
        QTC::TC("qpdf", "QPDFWriter extra header text add newline");
        this->extra_header_text += "\n";
    }
    else
    {
        QTC::TC("qpdf", "QPDFWriter extra header text no newline");
    }
}