AP4_Mp4vSampleEntry::AP4_Mp4vSampleEntry(AP4_UI16          width,
                                         AP4_UI16          height,
                                         AP4_UI16          depth,
                                         const char*       compressor_name,
                                         AP4_EsDescriptor* descriptor) :
    AP4_MpegVideoSampleEntry(AP4_ATOM_TYPE_MP4V, 
                             width, 
                             height, 
                             depth, 
                             compressor_name,
                             descriptor)
{
}