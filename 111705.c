DeepScanLineInputFile::DeepScanLineInputFile(InputPartData* part)
    
{

    _data = new Data(part->numThreads);
    _data->_deleteStream=false;
    _data->_streamData = part->mutex;
    _data->memoryMapped = _data->_streamData->is->isMemoryMapped();
    _data->version = part->version;

    try
    {
       initialize(part->header);
    }
    catch(...)
    {
        delete _data;
        throw;
    }
    _data->lineOffsets = part->chunkOffsets;

    _data->partNumber = part->partNumber;
}