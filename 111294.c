CompositeDeepScanLine::addSource(DeepScanLineInputFile* file)
{
    _Data->check_valid(file->header());
    _Data->_file.push_back(file);
}