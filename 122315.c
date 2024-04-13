QPDFWriter::pushEncryptionFilter()
{
    if (this->encrypted && (! this->cur_data_key.empty()))
    {
	Pipeline* p = 0;
	if (this->encrypt_use_aes)
	{
	    p = new Pl_AES_PDF(
		"aes stream encryption", this->pipeline, true,
		QUtil::unsigned_char_pointer(this->cur_data_key),
                this->cur_data_key.length());
	}
	else
	{
	    p = new Pl_RC4("rc4 stream encryption", this->pipeline,
			   QUtil::unsigned_char_pointer(this->cur_data_key),
			   this->cur_data_key.length());
	}
	pushPipeline(p);
    }
    // Must call this unconditionally so we can call popPipelineStack
    // to balance pushEncryptionFilter().
    activatePipelineStack();
}