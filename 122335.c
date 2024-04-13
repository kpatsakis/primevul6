QPDFWriter::initializePipelineStack(Pipeline *p)
{
    this->pipeline = new Pl_Count("qpdf count", p);
    to_delete.push_back(this->pipeline);
    this->pipeline_stack.push_back(this->pipeline);
}