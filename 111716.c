void DeepScanLineInputFile::readPixels (const char* rawPixelData, 
                                        const DeepFrameBuffer& frameBuffer, 
                                        int scanLine1, 
                                        int scanLine2) const
{
    //
    // read header from block - already converted from Xdr to native format
    //
    int data_scanline = *(int *) rawPixelData;
    uint64_t sampleCountTableDataSize=*(uint64_t *) (rawPixelData+4);
    uint64_t packedDataSize = *(uint64_t *) (rawPixelData+12);
    uint64_t unpackedDataSize = *(uint64_t *) (rawPixelData+20);

    
    
    //
    // Uncompress the data, if necessary
    //
    
    
    Compressor * decomp = NULL;
    const char * uncompressed_data;
    Compressor::Format format = Compressor::XDR;
    if(packedDataSize <unpackedDataSize)
    {
        decomp = newCompressor(_data->header.compression(),
                                             unpackedDataSize,
                                             _data->header);
                                             
        decomp->uncompress(rawPixelData+28+sampleCountTableDataSize,
                           static_cast<int>(packedDataSize),
                           data_scanline, uncompressed_data);
        format = decomp->format();
    }
    else
    {
        //
        // If the line is uncompressed, it's in XDR format,
        // regardless of the compressor's output format.
        //
        
        format = Compressor::XDR;
        uncompressed_data = rawPixelData+28+sampleCountTableDataSize;
    }
  
    
    int yStart, yStop, dy;
    
    if (_data->lineOrder == INCREASING_Y)
    {
        yStart = scanLine1;
        yStop = scanLine2 + 1;
        dy = 1;
    }
    else
    {
        yStart = scanLine2;
        yStop = scanLine1 - 1;
        dy = -1;
    }
    
    
    
    const char* samplecount_base = frameBuffer.getSampleCountSlice().base;
    int samplecount_xstride = static_cast<int>(frameBuffer.getSampleCountSlice().xStride);
    int samplecount_ystride = static_cast<int>(frameBuffer.getSampleCountSlice().yStride);
    
    //
    // For each line within the block, get the count of bytes.
    //
    
    int minYInLineBuffer = data_scanline;
    int maxYInLineBuffer = min(minYInLineBuffer + _data->linesInBuffer - 1, _data->maxY);
    
    vector<size_t> bytesPerLine(1+_data->maxY-_data->minY);
    
    
    bytesPerDeepLineTable (_data->header,
                           minYInLineBuffer,
                           maxYInLineBuffer,
                           samplecount_base,
                           samplecount_xstride,
                           samplecount_ystride,
                           bytesPerLine);
                           
    //
    // For each scanline within the block, get the offset.
    //
      
    vector<size_t> offsetInLineBuffer;
    offsetInLineBufferTable (bytesPerLine,
                             minYInLineBuffer - _data->minY,
                             maxYInLineBuffer - _data->minY,
                             _data->linesInBuffer,
                             offsetInLineBuffer);
                             
                             
    const ChannelList & channels=header().channels();    
    
    
    for (int y = yStart; y != yStop; y += dy)
    {
        
        const char *readPtr =uncompressed_data +
        offsetInLineBuffer[y - _data->minY];

        //
        // need to know the total number of samples on a scanline to skip channels
        // compute on demand: -1 means uncomputed
        //
        int lineSampleCount = -1;
        
        
        //
        // Iterate over all image channels in frame buffer
        //
    
    
        ChannelList::ConstIterator i = channels.begin();
                             
        for (DeepFrameBuffer::ConstIterator j = frameBuffer.begin();
                                            j != frameBuffer.end();
             ++j)
        {
            while (i != channels.end() && strcmp (i.name(), j.name()) < 0)
            {
                //
                // Channel i is present in the file but not
                // in the frame buffer; skip
                    
                if(lineSampleCount==-1)
                {
                     lineSampleCount=0;
                     const char * ptr = (samplecount_base+y*samplecount_ystride + samplecount_xstride*_data->minX);
                     for(int x=_data->minX;x<=_data->maxX;x++)
                     { 
                         
                          lineSampleCount+=*(const unsigned int *) ptr;
                          ptr+=samplecount_xstride;
                     }
                }

               skipChannel (readPtr, i.channel().type, lineSampleCount );
        
                ++i;
            }
                                 
            bool fill = false;
                                 
            if (i == channels.end() || strcmp (i.name(), j.name()) > 0)
            {
                //
                // Channel i is present in the frame buffer, but not in the file.
                // In the frame buffer, slice j will be filled with a default value.
                //
                                     
               fill = true;
            }
            if (modp (y, i.channel().ySampling) == 0)
            {        
                
                copyIntoDeepFrameBuffer (readPtr, j.slice().base,
                                         samplecount_base,
                                         samplecount_xstride,
                                         samplecount_ystride,
                                         y, _data->minX, _data->maxX,
                                         0, 0,
                                         0, 0,
                                         j.slice().sampleStride, 
                                         j.slice().xStride,
                                         j.slice().yStride,
                                         fill,
                                         j.slice().fillValue, 
                                         format,
                                         j.slice().type,
                                         i.channel().type);
                                         
                ++i;
                                         
            }
        }//next slice in framebuffer
    }//next row in image
    
    //
    // clean up
    //
    
    delete decomp;    
}