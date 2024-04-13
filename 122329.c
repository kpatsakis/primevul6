QPDFWriter::setOutputFilename(char const* filename)
{
    char const* description = filename;
    FILE* f = 0;
    if (filename == 0)
    {
	description = "standard output";
	QTC::TC("qpdf", "QPDFWriter write to stdout");
	f = stdout;
	QUtil::binary_stdout();
    }
    else
    {
	QTC::TC("qpdf", "QPDFWriter write to file");
	f = QUtil::safe_fopen(filename, "wb+");
	close_file = true;
    }
    setOutputFile(description, f, close_file);
}