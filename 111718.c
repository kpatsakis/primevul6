DeepScanLineInputFile::readPixelSampleCounts (int scanline1, int scanline2)
{
    uint64_t savedFilePos = 0;

    if(!_data->frameBufferValid)
    {
        throw IEX_NAMESPACE::ArgExc("readPixelSampleCounts called with no valid frame buffer");
    }
    
    try
    {
#if ILMTHREAD_THREADING_ENABLED
        std::lock_guard<std::mutex> lock (*_data->_streamData);
#endif
        savedFilePos = _data->_streamData->is->tellg();

        int scanLineMin = min (scanline1, scanline2);
        int scanLineMax = max (scanline1, scanline2);

        if (scanLineMin < _data->minY || scanLineMax > _data->maxY)
            throw IEX_NAMESPACE::ArgExc ("Tried to read scan line sample counts outside "
                               "the image file's data window.");

        for (int i = scanLineMin; i <= scanLineMax; i++)
        {
            //
            // if scanline is already read, it'll be in the cache
            // otherwise, read from file, store in cache and in caller's framebuffer
            //
            if (_data->gotSampleCount[i - _data->minY])
            {
                fillSampleCountFromCache(i,_data);
                                         
            }else{

                int lineBlockId = ( i - _data->minY ) / _data->linesInBuffer;

                readSampleCountForLineBlock ( _data->_streamData, _data, lineBlockId );

                int minYInLineBuffer = lineBlockId * _data->linesInBuffer + _data->minY;
                int maxYInLineBuffer = min ( minYInLineBuffer + _data->linesInBuffer - 1, _data->maxY );

                //
                // For each line within the block, get the count of bytes.
                //

                bytesPerDeepLineTable ( _data->header,
                                        minYInLineBuffer,
                                        maxYInLineBuffer,
                                        _data->sampleCountSliceBase,
                                        _data->sampleCountXStride,
                                        _data->sampleCountYStride,
                                        _data->bytesPerLine );

                //
                // For each scanline within the block, get the offset.
                //

                offsetInLineBufferTable ( _data->bytesPerLine,
                                          minYInLineBuffer - _data->minY,
                                          maxYInLineBuffer - _data->minY,
                                          _data->linesInBuffer,
                                          _data->offsetInLineBuffer );
            }
        }

        _data->_streamData->is->seekg(savedFilePos);
    }
    catch (IEX_NAMESPACE::BaseExc &e)
    {
        REPLACE_EXC (e, "Error reading sample count data from image "
                     "file \"" << fileName() << "\". " << e.what());

        _data->_streamData->is->seekg(savedFilePos);

        throw;
    }
}