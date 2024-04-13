DeepScanLineInputFile::lastScanLineInChunk(int y) const
{
    int minY = firstScanLineInChunk(y);
    return min(minY+_data->linesInBuffer-1,_data->maxY);
}