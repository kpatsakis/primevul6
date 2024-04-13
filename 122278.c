QPDFWriter::setOutputFile(char const* description, FILE* file, bool close_file)
{
    this->filename = description;
    this->file = file;
    this->close_file = close_file;
    Pipeline* p = new Pl_StdioFile("qpdf output", file);
    to_delete.push_back(p);
    initializePipelineStack(p);
}