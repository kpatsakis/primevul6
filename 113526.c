AP4_HevcSampleEntry::AP4_HevcSampleEntry(AP4_UI32            format,
                                         AP4_UI16            width,
                                         AP4_UI16            height,
                                         AP4_UI16            depth,
                                         const char*         compressor_name,
                                         const AP4_AtomParent* details) :
    AP4_VisualSampleEntry(format,
                          width, 
                          height, 
                          depth, 
                          compressor_name,
                          details)
{
}