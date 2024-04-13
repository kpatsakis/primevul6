RecordPadAlign(int size, int align)
{
    return (align - (size & (align - 1))) & (align - 1);
}                               /* RecordPadAlign */