QPDFWriter::setMinimumPDFVersion(std::string const& version,
                                 int extension_level)
{
    bool set_version = false;
    bool set_extension_level = false;
    if (this->min_pdf_version.empty())
    {
	set_version = true;
        set_extension_level = true;
    }
    else
    {
	int old_major = 0;
	int old_minor = 0;
	int min_major = 0;
	int min_minor = 0;
	parseVersion(version, old_major, old_minor);
	parseVersion(this->min_pdf_version, min_major, min_minor);
        int compare = compareVersions(
            old_major, old_minor, min_major, min_minor);
	if (compare > 0)
	{
	    QTC::TC("qpdf", "QPDFWriter increasing minimum version",
                    extension_level == 0 ? 0 : 1);
	    set_version = true;
            set_extension_level = true;
	}
        else if (compare == 0)
        {
            if (extension_level > this->min_extension_level)
            {
                QTC::TC("qpdf", "QPDFWriter increasing extension level");
                set_extension_level = true;
            }
	}
    }

    if (set_version)
    {
	this->min_pdf_version = version;
    }
    if (set_extension_level)
    {
        this->min_extension_level = extension_level;
    }
}