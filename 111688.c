readPixelData (InputStreamMutex *streamData,
               DeepScanLineInputFile::Data *ifd,
               int minY,
               char *&buffer,
               uint64_t &packedDataSize,
               uint64_t &unpackedDataSize)
{
    //
    // Read a single line buffer from the input file.
    //
    // If the input file is not memory-mapped, we copy the pixel data into
    // into the array pointed to by buffer.  If the file is memory-mapped,
    // then we change where buffer points to instead of writing into the
    // array (hence buffer needs to be a reference to a char *).
    //

    int lineBufferNumber = (minY - ifd->minY) / ifd->linesInBuffer;

    uint64_t lineOffset = ifd->lineOffsets[lineBufferNumber];

    if (lineOffset == 0)
        THROW (IEX_NAMESPACE::InputExc, "Scan line " << minY << " is missing.");

    //
    // Seek to the start of the scan line in the file,
    // if necessary.
    //

    if (!isMultiPart(ifd->version))
    {
        if (ifd->nextLineBufferMinY != minY)
            streamData->is->seekg (lineOffset);
    }
    else
    {
        //
        // In a multi-part file, the file pointer may have been moved by
        // other parts, so we have to ask tellg() where we are.
        //
        if (streamData->is->tellg() != ifd->lineOffsets[lineBufferNumber])
            streamData->is->seekg (lineOffset);
    }

    //
    // Read the data block's header.
    //

    int yInFile;

    //
    // Read the part number when we are dealing with a multi-part file.
    //

    if (isMultiPart(ifd->version))
    {
        int partNumber;
        OPENEXR_IMF_INTERNAL_NAMESPACE::Xdr::read <OPENEXR_IMF_INTERNAL_NAMESPACE::StreamIO> (*streamData->is, partNumber);
        if (partNumber != ifd->partNumber)
        {
            THROW (IEX_NAMESPACE::ArgExc, "Unexpected part number " << partNumber
                   << ", should be " << ifd->partNumber << ".");
        }
    }

    OPENEXR_IMF_INTERNAL_NAMESPACE::Xdr::read <OPENEXR_IMF_INTERNAL_NAMESPACE::StreamIO> (*streamData->is, yInFile);

    if (yInFile != minY)
        throw IEX_NAMESPACE::InputExc ("Unexpected data block y coordinate.");

    uint64_t sampleCountTableSize;
    OPENEXR_IMF_INTERNAL_NAMESPACE::Xdr::read <OPENEXR_IMF_INTERNAL_NAMESPACE::StreamIO> (*streamData->is, sampleCountTableSize);
    OPENEXR_IMF_INTERNAL_NAMESPACE::Xdr::read <OPENEXR_IMF_INTERNAL_NAMESPACE::StreamIO> (*streamData->is, packedDataSize);
    OPENEXR_IMF_INTERNAL_NAMESPACE::Xdr::read <OPENEXR_IMF_INTERNAL_NAMESPACE::StreamIO> (*streamData->is, unpackedDataSize);


    //
    // We make a check on the data size requirements here.
    // Whilst we wish to store 64bit sizes on disk, not all the compressors
    // have been made to work with such data sizes and are still limited to
    // using signed 32 bit (int) for the data size. As such, this version
    // insists that we validate that the data size does not exceed the data
    // type max limit.
    // @TODO refactor the compressor code to ensure full 64-bit support.
    //

    int compressorMaxDataSize = std::numeric_limits<int>::max();
    if (packedDataSize   > uint64_t(compressorMaxDataSize) ||
        unpackedDataSize > uint64_t(compressorMaxDataSize))
    {
        THROW (IEX_NAMESPACE::ArgExc, "This version of the library does not support "
              << "the allocation of data with size  > " << compressorMaxDataSize
              << " file unpacked size :" << unpackedDataSize
              << " file packed size   :" << packedDataSize << ".\n");
    }

    //
    // Skip the pixel sample count table because we have read this data.
    //

    Xdr::skip <StreamIO> (*streamData->is, static_cast<int>(sampleCountTableSize));

    //
    // Read the pixel data.
    //

    if (streamData->is->isMemoryMapped ())
        buffer = streamData->is->readMemoryMapped (static_cast<int>(packedDataSize));
    else
    {
        // (TODO) check if the packed data size is too big?
        // (TODO) better memory management. Don't delete buffer all the time.
        if (buffer != 0) delete[] buffer;
        buffer = new char[packedDataSize];
        streamData->is->read (buffer, static_cast<int>(packedDataSize));
    }

    //
    // Keep track of which scan line is the next one in
    // the file, so that we can avoid redundant seekg()
    // operations (seekg() can be fairly expensive).
    //

    if (ifd->lineOrder == INCREASING_Y)
        ifd->nextLineBufferMinY = minY + ifd->linesInBuffer;
    else
        ifd->nextLineBufferMinY = minY - ifd->linesInBuffer;
}