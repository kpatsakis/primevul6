DeepScanLineInputFile::Data::~Data ()
{
    for (size_t i = 0; i < lineBuffers.size(); i++)
        if (lineBuffers[i] != 0)
            delete lineBuffers[i];

    for (size_t i = 0; i < slices.size(); i++)
        delete slices[i];

    if (sampleCountTableComp != 0)
        delete sampleCountTableComp;
    
    if (multiPartBackwardSupport)
        delete multiPartFile;
}