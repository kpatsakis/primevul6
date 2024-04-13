bool  PxMEncoder::isFormatSupported( int depth ) const
{
    return depth == CV_8U || depth == CV_16U;
}