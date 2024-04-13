fillSampleCountFromCache(int y, DeepScanLineInputFile::Data* data)
{
    int yInDataWindow = y - data->minY;
    char* base = data->sampleCountSliceBase;
    int xStride = data->sampleCountXStride;
    int yStride = data->sampleCountYStride;
    
    for (int x = data->minX; x <= data->maxX; x++)
    {
        unsigned int count = data->sampleCount[yInDataWindow][x - data->minX];    
        sampleCount(base, xStride, yStride, x, y) = count;
    }
}