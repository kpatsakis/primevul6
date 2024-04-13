QPDFWriter::forcePDFVersion(std::string const& version,
                            int extension_level)
{
    this->forced_pdf_version = version;
    this->forced_extension_level = extension_level;
}