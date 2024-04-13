AP4_Mp4vSampleEntry::AP4_Mp4vSampleEntry(AP4_Size         size,
                                         AP4_ByteStream&  stream,
                                         AP4_AtomFactory& atom_factory) :
    AP4_MpegVideoSampleEntry(AP4_ATOM_TYPE_MP4V, size, stream, atom_factory)
{
}