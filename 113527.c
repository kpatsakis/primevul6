AP4_MpegAudioSampleEntry::AP4_MpegAudioSampleEntry(
    AP4_UI32         type,
    AP4_Size         size,
    AP4_ByteStream&  stream,
    AP4_AtomFactory& atom_factory) :
    AP4_AudioSampleEntry(type, size, stream, atom_factory)
{
}