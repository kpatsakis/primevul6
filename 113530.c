AP4_SampleEntry::WriteFields(AP4_ByteStream& stream)
{
    AP4_Result result;
    
    // reserved1
    result = stream.Write(m_Reserved1, sizeof(m_Reserved1));
    if (AP4_FAILED(result)) return result;

    // data reference index
    result = stream.WriteUI16(m_DataReferenceIndex);
    if (AP4_FAILED(result)) return result;
    
    return result;
}