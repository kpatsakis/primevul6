QPDFWriter::popPipelineStack(PointerHolder<Buffer>* bp)
{
    assert(this->pipeline_stack.size() >= 2);
    this->pipeline->finish();
    assert(dynamic_cast<Pl_Count*>(this->pipeline_stack.back()) ==
	   this->pipeline);
    delete this->pipeline_stack.back();
    this->pipeline_stack.pop_back();
    while (dynamic_cast<Pl_Count*>(this->pipeline_stack.back()) == 0)
    {
	Pipeline* p = this->pipeline_stack.back();
        if (dynamic_cast<Pl_MD5*>(p) == this->md5_pipeline)
        {
            this->md5_pipeline = 0;
        }
	this->pipeline_stack.pop_back();
	Pl_Buffer* buf = dynamic_cast<Pl_Buffer*>(p);
	if (bp && buf)
	{
	    *bp = buf->getBuffer();
	}
	delete p;
    }
    this->pipeline = dynamic_cast<Pl_Count*>(this->pipeline_stack.back());
}