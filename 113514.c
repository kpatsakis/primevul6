AP4_MpegSystemSampleEntry::AP4_MpegSystemSampleEntry(
    AP4_UI32         type,
    AP4_Size         size,
    AP4_ByteStream&  stream,
    AP4_AtomFactory& atom_factory) :
    AP4_SampleEntry(type, size, stream, atom_factory)
{
}