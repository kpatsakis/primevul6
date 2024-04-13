AP4_RtpHintSampleEntry::ReadFields(AP4_ByteStream& stream)
{
    // sample entry
    AP4_Result result = AP4_SampleEntry::ReadFields(stream);
    if (result < 0) return result;

    // data
    result = stream.ReadUI16(m_HintTrackVersion);
    if (AP4_FAILED(result)) return result;
    result = stream.ReadUI16(m_HighestCompatibleVersion);
    if (AP4_FAILED(result)) return result;
    result = stream.ReadUI32(m_MaxPacketSize);
    if (AP4_FAILED(result)) return result;

    return AP4_SUCCESS;
}