QPDFWriter::setOutputPipeline(Pipeline* p)
{
    this->filename = "custom pipeline";
    initializePipelineStack(p);
}