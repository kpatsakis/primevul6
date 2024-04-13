QPDFWriter::pushMD5Pipeline()
{
    if (! this->id2.empty())
    {
        // Can't happen in the code
        throw std::logic_error(
            "Deterministic ID computation enabled after ID"
            " generation has already occurred.");
    }
    assert(this->deterministic_id);
    assert(this->md5_pipeline == 0);
    assert(this->pipeline->getCount() == 0);
    this->md5_pipeline = new Pl_MD5("qpdf md5", this->pipeline);
    this->md5_pipeline->persistAcrossFinish(true);
    // Special case code in popPipelineStack clears this->md5_pipeline
    // upon deletion.
    pushPipeline(this->md5_pipeline);
    activatePipelineStack();
}