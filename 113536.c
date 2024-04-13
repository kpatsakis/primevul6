AP4_Mp4sSampleEntry::AP4_Mp4sSampleEntry(AP4_Size         size,
                                         AP4_ByteStream&  stream,
                                         AP4_AtomFactory& atom_factory) :
    AP4_MpegSystemSampleEntry(AP4_ATOM_TYPE_MP4S, size, stream, atom_factory)
{
}