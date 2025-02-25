DeepScanLineInputFile::DeepScanLineInputFile
    (const Header &header,
     OPENEXR_IMF_INTERNAL_NAMESPACE::IStream *is,
     int version,
     int numThreads)
:
    _data (new Data (numThreads))
{
    _data->_streamData=new InputStreamMutex();
    _data->_deleteStream=false;
    _data->_streamData->is = is;
    
    _data->memoryMapped = is->isMemoryMapped();

    _data->version =version;
    
    try
    {
        initialize (header);
    }
    catch (...)
    {
        if (_data && _data->_streamData)
        {
            delete _data->_streamData;
        }
        if (_data)       delete _data;

        throw;
   }

    readLineOffsets (*_data->_streamData->is,
                     _data->lineOrder,
                     _data->lineOffsets,
                     _data->fileIsComplete);
}