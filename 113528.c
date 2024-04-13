AP4_RtpHintSampleEntry::WriteFields(AP4_ByteStream& stream)
{
    // sample entry
    AP4_Result result = AP4_SampleEntry::WriteFields(stream);
    if (AP4_FAILED(result)) return result;
    
    // data
    result = stream.WriteUI16(m_HintTrackVersion);
    if (AP4_FAILED(result)) return result;
    result = stream.WriteUI16(m_HighestCompatibleVersion);
    if (AP4_FAILED(result)) return result;
    result = stream.WriteUI32(m_MaxPacketSize);
    if (AP4_FAILED(result)) return result;

    return result;
}