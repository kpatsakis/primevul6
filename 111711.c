DeepScanLineInputFile::Data::Data (int numThreads):
        partNumber(-1),
        numThreads(numThreads),
        multiPartBackwardSupport(false),
        multiPartFile(NULL),
        memoryMapped(false),
        frameBufferValid(false),
        _streamData(NULL),
        _deleteStream(false)
{
    //
    // We need at least one lineBuffer, but if threading is used,
    // to keep n threads busy we need 2*n lineBuffers
    //

    lineBuffers.resize (max (1, 2 * numThreads));

    for (size_t i = 0; i < lineBuffers.size(); i++)
        lineBuffers[i] = 0;

    sampleCountTableComp = 0;
}