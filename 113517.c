AP4_MpegSystemSampleEntry::AP4_MpegSystemSampleEntry(
    AP4_UI32          type,
    AP4_EsDescriptor* descriptor) :
    AP4_SampleEntry(type)
{
    if (descriptor) AddChild(new AP4_EsdsAtom(descriptor));
}