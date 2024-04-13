QPDFWriter::computeDeterministicIDData()
{
    assert(this->md5_pipeline != 0);
    assert(this->deterministic_id_data.empty());
    this->deterministic_id_data = this->md5_pipeline->getHexDigest();
    this->md5_pipeline->enable(false);
}