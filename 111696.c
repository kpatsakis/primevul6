void DeepScanLineInputFile::multiPartInitialize(InputPartData* part)
{
    
    _data->_streamData = part->mutex;
    _data->memoryMapped = _data->_streamData->is->isMemoryMapped();
    _data->version = part->version;
    
    initialize(part->header);
    
    _data->lineOffsets = part->chunkOffsets;
    
    _data->partNumber = part->partNumber;
    
}