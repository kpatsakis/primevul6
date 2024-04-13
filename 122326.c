QPDFWriter::~QPDFWriter()
{
    if (file && close_file)
    {
	fclose(file);
    }
    if (output_buffer)
    {
	delete output_buffer;
    }
}