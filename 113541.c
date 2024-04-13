AP4_MpegVideoSampleEntry::AP4_MpegVideoSampleEntry(
    AP4_UI32          type,
    AP4_UI16          width,
    AP4_UI16          height,
    AP4_UI16          depth,
    const char*       compressor_name,
    AP4_EsDescriptor* descriptor) :
    AP4_VisualSampleEntry(type, 
                          width, 
                          height, 
                          depth, 
                          compressor_name)
{
    if (descriptor) AddChild(new AP4_EsdsAtom(descriptor));
}