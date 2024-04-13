QPDFWriter::QPDFWriter(QPDF& pdf, char const* description,
                       FILE *file, bool close_file) :
    pdf(pdf)
{
    init();
    setOutputFile(description, file, close_file);
}