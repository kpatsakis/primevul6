AP4_AudioSampleEntry::WriteFields(AP4_ByteStream& stream)
{
    AP4_Result result;
    
    // write the fields of the base class
    result = AP4_SampleEntry::WriteFields(stream);
    if (AP4_FAILED(result)) return result;

    // QT version
    result = stream.WriteUI16(m_QtVersion);
    if (AP4_FAILED(result)) return result;

    // QT revision
    result = stream.WriteUI16(m_QtRevision);
    if (AP4_FAILED(result)) return result;

    // QT vendor
    result = stream.WriteUI32(m_QtVendor);
    if (AP4_FAILED(result)) return result;

    // channel count
    result = stream.WriteUI16(m_ChannelCount);
    if (AP4_FAILED(result)) return result;
    
    // sample size 
    result = stream.WriteUI16(m_SampleSize);
    if (AP4_FAILED(result)) return result;

    // QT compression ID
    result = stream.WriteUI16(m_QtCompressionId);
    if (AP4_FAILED(result)) return result;

    // QT packet size
    result = stream.WriteUI16(m_QtPacketSize);
    if (AP4_FAILED(result)) return result;

    // sample rate
    result = stream.WriteUI32(m_SampleRate);
    if (AP4_FAILED(result)) return result;

    if (m_QtVersion == 1) {
        result = stream.WriteUI32(m_QtV1SamplesPerPacket);
        if (AP4_FAILED(result)) return result;
        result = stream.WriteUI32(m_QtV1BytesPerPacket);
        if (AP4_FAILED(result)) return result;
        result = stream.WriteUI32(m_QtV1BytesPerFrame);
        if (AP4_FAILED(result)) return result;
        result = stream.WriteUI32(m_QtV1BytesPerSample);
        if (AP4_FAILED(result)) return result;
    } else if (m_QtVersion == 2) {
        stream.WriteUI32(m_QtV2StructSize);
        stream.WriteDouble(m_QtV2SampleRate64);
        stream.WriteUI32(m_QtV2ChannelCount);
        stream.WriteUI32(m_QtV2Reserved);
        stream.WriteUI32(m_QtV2BitsPerChannel);
        stream.WriteUI32(m_QtV2FormatSpecificFlags);
        stream.WriteUI32(m_QtV2BytesPerAudioPacket);
        stream.WriteUI32(m_QtV2LPCMFramesPerAudioPacket);
        if (m_QtV2Extension.GetDataSize()) {
            stream.Write(m_QtV2Extension.GetData(),
                         m_QtV2Extension.GetDataSize());
        }
    }
    
    return result;
}