QPDFWriter::QPDFWriter(QPDF& pdf, char const* filename) :
    pdf(pdf)
{
    init();
    setOutputFilename(filename);
}