DeepScanLineInputFile::frameBuffer () const
{
#if ILMTHREAD_THREADING_ENABLED
    std::lock_guard<std::mutex> lock (*_data->_streamData);
#endif
    return _data->frameBuffer;
}