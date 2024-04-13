LineBufferTask::LineBufferTask
    (TaskGroup *group,
     DeepScanLineInputFile::Data *ifd,
     LineBuffer *lineBuffer,
     int scanLineMin,
     int scanLineMax)
:
    Task (group),
    _ifd (ifd),
    _lineBuffer (lineBuffer),
    _scanLineMin (scanLineMin),
    _scanLineMax (scanLineMax)
{
    // empty
}