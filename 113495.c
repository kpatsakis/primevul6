AP4_MpegAudioSampleEntry::AP4_MpegAudioSampleEntry(
    AP4_UI32          type,
    AP4_UI32          sample_rate, 
    AP4_UI16          sample_size,
    AP4_UI16          channel_count,
    AP4_EsDescriptor* descriptor) :
    AP4_AudioSampleEntry(type, sample_rate, sample_size, channel_count)
{
    if (descriptor) AddChild(new AP4_EsdsAtom(descriptor));
}