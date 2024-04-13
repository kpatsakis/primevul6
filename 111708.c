LineBuffer::~LineBuffer ()
{
    if (compressor != 0)
        delete compressor;
}