QPDFWriter::setDataKey(int objid)
{
    this->cur_data_key = QPDF::compute_data_key(
	this->encryption_key, objid, 0,
        this->encrypt_use_aes, this->encryption_V, this->encryption_R);
}