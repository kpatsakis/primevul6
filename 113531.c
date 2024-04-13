AP4_UnknownSampleEntry::ReadFields(AP4_ByteStream& stream)
{
    // sample entry
    AP4_Result result = AP4_SampleEntry::ReadFields(stream);
    if (AP4_FAILED(result)) return result;
    
    // read the payload
    return stream.Read(m_Payload.UseData(), m_Payload.GetDataSize());
}