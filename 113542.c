AP4_Mp4aSampleEntry::AP4_Mp4aSampleEntry(AP4_Size         size,
                                         AP4_ByteStream&  stream,
                                         AP4_AtomFactory& atom_factory) :
    AP4_MpegAudioSampleEntry(AP4_ATOM_TYPE_MP4A, size, stream, atom_factory)
{
}