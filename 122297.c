QPDFWriter::writeHeader()
{
    setMinimumPDFVersion(pdf.getPDFVersion(), pdf.getExtensionLevel());
    this->final_pdf_version = this->min_pdf_version;
    this->final_extension_level = this->min_extension_level;
    if (! this->forced_pdf_version.empty())
    {
	QTC::TC("qpdf", "QPDFWriter using forced PDF version");
	this->final_pdf_version = this->forced_pdf_version;
        this->final_extension_level = this->forced_extension_level;
    }

    writeString("%PDF-");
    writeString(this->final_pdf_version);
    // This string of binary characters would not be valid UTF-8, so
    // it really should be treated as binary.
    writeString("\n%\xbf\xf7\xa2\xfe\n");
    writeStringQDF("%QDF-1.0\n\n");

    // Note: do not write extra header text here.  Linearized PDFs
    // must include the entire linearization parameter dictionary
    // within the first 1024 characters of the PDF file, so for
    // linearized files, we have to write extra header text after the
    // linearization parameter dictionary.
}