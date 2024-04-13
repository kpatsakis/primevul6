void DeepScanLineInputFile::readPixelSampleCounts (const char* rawPixelData, 
                                                   const DeepFrameBuffer& frameBuffer, 
                                                   int scanLine1, 
                                                   int scanLine2) const
{
    //
    // read header from block - already converted from Xdr to native format
    //
    int data_scanline = *(int *) rawPixelData;
    uint64_t sampleCountTableDataSize=*(uint64_t *) (rawPixelData+4);
    
    
    int maxY;
    maxY = min(data_scanline + _data->linesInBuffer - 1, _data->maxY);
    
    if(scanLine1 != data_scanline)
    {
        THROW(IEX_NAMESPACE::ArgExc,"readPixelSampleCounts(rawPixelData,frameBuffer,"<< scanLine1 << ',' << scanLine2 << ") called with incorrect start scanline - should be " << data_scanline );
    }
    
    if(scanLine2 != maxY)
    {
        THROW(IEX_NAMESPACE::ArgExc,"readPixelSampleCounts(rawPixelData,frameBuffer,"<< scanLine1 << ',' << scanLine2 << ") called with incorrect end scanline - should be " << maxY );
    }
    
    
    //
    // If the sample count table is compressed, we'll uncompress it.
    //
    
    uint64_t rawSampleCountTableSize = (maxY - data_scanline + 1) * (_data->maxX - _data->minX + 1) *
    Xdr::size <unsigned int> ();
    
    
    Compressor * decomp=NULL;
    const char* readPtr;
    if (sampleCountTableDataSize < rawSampleCountTableSize)
    {
        decomp = newCompressor(_data->header.compression(),
                               rawSampleCountTableSize,
                               _data->header);
                                                    
        decomp->uncompress(rawPixelData+28,
                                               static_cast<int>(sampleCountTableDataSize),
                                               data_scanline,
                                               readPtr);
    }
    else readPtr = rawPixelData+28;
    
    char* base = frameBuffer.getSampleCountSlice().base;
    int xStride = static_cast<int>(frameBuffer.getSampleCountSlice().xStride);
    int yStride = static_cast<int>(frameBuffer.getSampleCountSlice().yStride);
    
   
    
    for (int y = scanLine1; y <= scanLine2; y++)
    {
        int lastAccumulatedCount = 0;
        for (int x = _data->minX; x <= _data->maxX; x++)
        {
            int accumulatedCount, count;
            
            //
            // Read the sample count for pixel (x, y).
            //
            
            Xdr::read <CharPtrIO> (readPtr, accumulatedCount);
            if (x == _data->minX)
                count = accumulatedCount;
            else
                count = accumulatedCount - lastAccumulatedCount;
            lastAccumulatedCount = accumulatedCount;
            
            //
            // Store the data in both internal and external data structure.
            //
            
            sampleCount(base, xStride, yStride, x, y) = count;
        }
    }
    
    if(decomp)
    {
       delete decomp;
    }
}