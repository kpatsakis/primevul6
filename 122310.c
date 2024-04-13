QPDFWriter::setOutputMemory()
{
    this->filename = "memory buffer";
    this->buffer_pipeline = new Pl_Buffer("qpdf output");
    to_delete.push_back(this->buffer_pipeline);
    initializePipelineStack(this->buffer_pipeline);
}