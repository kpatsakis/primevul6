LineBuffer::LineBuffer ():
    uncompressedData (0),
    buffer (0),
    packedDataSize (0),
    compressor (0),
    format (defaultFormat(compressor)),
    number (-1),
    hasException (false),
    exception (),
    _sem (1)
{
    // empty
}