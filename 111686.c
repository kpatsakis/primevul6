DeepScanLineInputFile::rawPixelData (int firstScanLine,
                                     char *pixelData,
                                     uint64_t &pixelDataSize)
{
   
    
    int minY = lineBufferMinY
    (firstScanLine, _data->minY, _data->linesInBuffer);
    int lineBufferNumber = (minY - _data->minY) / _data->linesInBuffer;
    
    uint64_t lineOffset = _data->lineOffsets[lineBufferNumber];
    
    if (lineOffset == 0)
        THROW (IEX_NAMESPACE::InputExc, "Scan line " << minY << " is missing.");
    
    
#if ILMTHREAD_THREADING_ENABLED
    // enter the lock here - prevent another thread reseeking the file during read
    std::lock_guard<std::mutex> lock (*_data->_streamData);
#endif
    //
    // Seek to the start of the scan line in the file,
    //
    
    if (_data->_streamData->is->tellg() != _data->lineOffsets[lineBufferNumber])
        _data->_streamData->is->seekg (lineOffset);
    
    //
    // Read the data block's header.
    //
    
    int yInFile;
    
    //
    // Read the part number when we are dealing with a multi-part file.
    //
    
    if (isMultiPart(_data->version))
    {
        int partNumber;
        OPENEXR_IMF_INTERNAL_NAMESPACE::Xdr::read <OPENEXR_IMF_INTERNAL_NAMESPACE::StreamIO> (*_data->_streamData->is, partNumber);
        if (partNumber != _data->partNumber)
        {
            THROW (IEX_NAMESPACE::ArgExc, "Unexpected part number " << partNumber
            << ", should be " << _data->partNumber << ".");
        }
    }
    
    OPENEXR_IMF_INTERNAL_NAMESPACE::Xdr::read <OPENEXR_IMF_INTERNAL_NAMESPACE::StreamIO> (*_data->_streamData->is, yInFile);
    
    if (yInFile != minY)
        throw IEX_NAMESPACE::InputExc ("Unexpected data block y coordinate.");
    
    uint64_t sampleCountTableSize;
    uint64_t packedDataSize;
    OPENEXR_IMF_INTERNAL_NAMESPACE::Xdr::read <OPENEXR_IMF_INTERNAL_NAMESPACE::StreamIO> (*_data->_streamData->is, sampleCountTableSize);
    OPENEXR_IMF_INTERNAL_NAMESPACE::Xdr::read <OPENEXR_IMF_INTERNAL_NAMESPACE::StreamIO> (*_data->_streamData->is, packedDataSize);
    
    // total requirement for reading all the data
    
    uint64_t totalSizeRequired=28+sampleCountTableSize+packedDataSize;
    
    bool big_enough = totalSizeRequired<=pixelDataSize;
    
    pixelDataSize = totalSizeRequired;
    
    // was the block we were given big enough?
    if(!big_enough || pixelData==NULL)
    {        
        // special case: seek stream back to start if we are at the beginning (regular reading pixels assumes it doesn't need to seek
        // in single part files)
        if(!isMultiPart(_data->version))
        {
          if (_data->nextLineBufferMinY == minY)
              _data->_streamData->is->seekg (lineOffset);
        }
        // leave lock here - bail before reading more data
        return;
    }
    
    // copy the values we have read into the output block
    *(int *) pixelData = yInFile;
    bytesOruint64_t tmp;
    tmp.i=sampleCountTableSize;
    memcpy(pixelData+4,&tmp.b,8);
    tmp.i = packedDataSize;
    memcpy(pixelData+12,&tmp.b,8);

    // didn't read the unpackedsize - do that now
    Xdr::read<StreamIO> (*_data->_streamData->is,tmp.i);
    memcpy(pixelData+20,&tmp.b,8);

    // read the actual data
    _data->_streamData->is->read(pixelData+28, static_cast<int>(sampleCountTableSize+packedDataSize));
    
    // special case: seek stream back to start if we are at the beginning (regular reading pixels assumes it doesn't need to seek
    // in single part files)
    if(!isMultiPart(_data->version))
    {
        if (_data->nextLineBufferMinY == minY)
            _data->_streamData->is->seekg (lineOffset);
    }
    
    // leave lock here
    
}