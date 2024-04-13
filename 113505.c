AP4_UnknownSampleEntry::WriteFields(AP4_ByteStream& stream)
{
    AP4_Result result;
    
    // write the fields of the base class
    result = AP4_SampleEntry::WriteFields(stream);
    if (AP4_FAILED(result)) return result;
    
    // write the payload
    return stream.Write(m_Payload.GetData(), m_Payload.GetDataSize());
}