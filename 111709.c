LineBufferTask::execute ()
{
    try
    {
        //
        // Uncompress the data, if necessary
        //

        if (_lineBuffer->uncompressedData == 0)
        {
            uint64_t uncompressedSize = 0;
            int maxY = min (_lineBuffer->maxY, _ifd->maxY);

            for (int i = _lineBuffer->minY - _ifd->minY;
                 i <= maxY - _ifd->minY;
                 ++i)
            {
                uncompressedSize += (int) _ifd->bytesPerLine[i];
            }

            //
            // Create the compressor everytime when we want to use it,
            // because we don't know maxBytesPerLine beforehand.
            // (TODO) optimize this. don't do this every time.
            //

            if (_lineBuffer->compressor != 0)
                delete _lineBuffer->compressor;
            uint64_t maxBytesPerLine = 0;
            for (int i = _lineBuffer->minY - _ifd->minY;
                 i <= maxY - _ifd->minY;
                 ++i)
            {
                if (_ifd->bytesPerLine[i] > maxBytesPerLine)
                    maxBytesPerLine = _ifd->bytesPerLine[i];
            }
            _lineBuffer->compressor = newCompressor(_ifd->header.compression(),
                                                    maxBytesPerLine,
                                                    _ifd->header);

            if (_lineBuffer->compressor &&
                _lineBuffer->packedDataSize < uncompressedSize)
            {
                _lineBuffer->format = _lineBuffer->compressor->format();

                _lineBuffer->packedDataSize = _lineBuffer->compressor->uncompress
                    (_lineBuffer->buffer, static_cast<int>(_lineBuffer->packedDataSize),
                     _lineBuffer->minY, _lineBuffer->uncompressedData);
            }
            else
            {
                //
                // If the line is uncompressed, it's in XDR format,
                // regardless of the compressor's output format.
                //

                _lineBuffer->format = Compressor::XDR;
                _lineBuffer->uncompressedData = _lineBuffer->buffer;

                if(_lineBuffer->packedDataSize!=maxBytesPerLine)
                {
                    THROW (IEX_NAMESPACE::InputExc, "Incorrect size for uncompressed data. Expected " << maxBytesPerLine << " got " << _lineBuffer->packedDataSize << " bytes");
                }
            }
        }

        int yStart, yStop, dy;

        if (_ifd->lineOrder == INCREASING_Y)
        {
            yStart = _scanLineMin;
            yStop = _scanLineMax + 1;
            dy = 1;
        }
        else
        {
            yStart = _scanLineMax;
            yStop = _scanLineMin - 1;
            dy = -1;
        }

        for (int y = yStart; y != yStop; y += dy)
        {
            //
            // Convert one scan line's worth of pixel data back
            // from the machine-independent representation, and
            // store the result in the frame buffer.
            //

            const char *readPtr = _lineBuffer->uncompressedData +
                                  _ifd->offsetInLineBuffer[y - _ifd->minY];

            //
            // Iterate over all image channels.
            //

            for (unsigned int i = 0; i < _ifd->slices.size(); ++i)
            {
                //
                // Test if scan line y of this channel contains any data
                // (the scan line contains data only if y % ySampling == 0).
                //

                InSliceInfo &slice = *_ifd->slices[i];

                if (modp (y, slice.ySampling) != 0)
                    continue;

                //
                // Find the x coordinates of the leftmost and rightmost
                // sampled pixels (i.e. pixels within the data window
                // for which x % xSampling == 0).
                //

                //
                // Fill the frame buffer with pixel data.
                //

                if (slice.skip)
                {
                    //
                    // The file contains data for this channel, but
                    // the frame buffer contains no slice for this channel.
                    //

                    skipChannel (readPtr, slice.typeInFile,
                                 _ifd->lineSampleCount[y - _ifd->minY]);
                }
                else
                {
                    //
                    // The frame buffer contains a slice for this channel.
                    //

                    int width = (_ifd->maxX - _ifd->minX + 1);

                    ptrdiff_t base = reinterpret_cast<ptrdiff_t>(&_ifd->sampleCount[0][0]);
                    base -= sizeof(unsigned int)*_ifd->minX;
                    base -= sizeof(unsigned int)*static_cast<ptrdiff_t>(_ifd->minY) * static_cast<ptrdiff_t>(width);

                    copyIntoDeepFrameBuffer (readPtr, slice.base,
                                             reinterpret_cast<char*>(base),
                                             sizeof(unsigned int) * 1,
                                             sizeof(unsigned int) * width,
                                             y, _ifd->minX, _ifd->maxX,
                                             0, 0,
                                             0, 0,
                                             slice.sampleStride, 
                                             slice.xPointerStride,
                                             slice.yPointerStride,
                                             slice.fill,
                                             slice.fillValue, _lineBuffer->format,
                                             slice.typeInFrameBuffer,
                                             slice.typeInFile);
                }
            }
        }
    }
    catch (std::exception &e)
    {
        if (!_lineBuffer->hasException)
        {
            _lineBuffer->exception = e.what();
            _lineBuffer->hasException = true;
        }
    }
    catch (...)
    {
        if (!_lineBuffer->hasException)
        {
            _lineBuffer->exception = "unrecognized exception";
            _lineBuffer->hasException = true;
        }
    }
}