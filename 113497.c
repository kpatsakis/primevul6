AP4_AudioSampleEntry::AP4_AudioSampleEntry(AP4_Atom::Type format,
                                           AP4_UI32       sample_rate,
                                           AP4_UI16       sample_size,
                                           AP4_UI16       channel_count) :
    AP4_SampleEntry(format),
    m_QtVersion(0),
    m_QtRevision(0),
    m_QtVendor(0),
    m_ChannelCount(channel_count),
    m_SampleSize(sample_size),
    m_QtCompressionId(0),
    m_QtPacketSize(0),
    m_SampleRate(sample_rate),
    m_QtV1SamplesPerPacket(0),
    m_QtV1BytesPerPacket(0),
    m_QtV1BytesPerFrame(0),
    m_QtV1BytesPerSample(0),
    m_QtV2StructSize(0),
    m_QtV2SampleRate64(0.0),
    m_QtV2ChannelCount(0),
    m_QtV2Reserved(0),
    m_QtV2BitsPerChannel(0),
    m_QtV2FormatSpecificFlags(0),
    m_QtV2BytesPerAudioPacket(0),
    m_QtV2LPCMFramesPerAudioPacket(0)    
{
    m_Size32 += 20;
}