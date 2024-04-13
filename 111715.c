DeepScanLineInputFile::firstScanLineInChunk(int y) const
{
    return int((y-_data->minY)/_data->linesInBuffer)*_data->linesInBuffer + _data->minY;
}