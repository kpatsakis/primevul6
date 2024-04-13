AP4_SubtitleSampleEntry::AP4_SubtitleSampleEntry(AP4_Atom::Type   format,
                                                 AP4_Size         size,
                                                 AP4_ByteStream&  stream,
                                                 AP4_AtomFactory& atom_factory) :
    AP4_SampleEntry(format, size)
{
    Read(stream, atom_factory);
}