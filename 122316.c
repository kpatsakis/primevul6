QPDFWriter::pushPipeline(Pipeline* p)
{
    assert(dynamic_cast<Pl_Count*>(p) == 0);
    this->pipeline_stack.push_back(p);
    return p;
}