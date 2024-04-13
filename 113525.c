AP4_Mp4aSampleEntry::AP4_Mp4aSampleEntry(AP4_UI32          sample_rate, 
                                         AP4_UI16          sample_size,
                                         AP4_UI16          channel_count,
                                         AP4_EsDescriptor* descriptor) :
    AP4_MpegAudioSampleEntry(AP4_ATOM_TYPE_MP4A, 
                             sample_rate, 
                             sample_size, 
                             channel_count,
                             descriptor)
{
}