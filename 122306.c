QPDFWriter::activatePipelineStack()
{
    Pl_Count* c = new Pl_Count("count", this->pipeline_stack.back());
    this->pipeline_stack.push_back(c);
    this->pipeline = c;
}