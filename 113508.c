AP4_SubtitleSampleEntry::WriteFields(AP4_ByteStream& stream)
{
    AP4_Result result;
        
    // write the fields of the base class
    result = AP4_SampleEntry::WriteFields(stream);
    if (AP4_FAILED(result)) return result;
    
    // write fields from this class
    result = stream.WriteString(m_Namespace.GetChars());
    if (AP4_FAILED(result)) return result;
    result = stream.WriteUI08(0);
    if (AP4_FAILED(result)) return result;
    result = stream.WriteString(m_SchemaLocation.GetChars());
    if (AP4_FAILED(result)) return result;
    result = stream.WriteUI08(0);
    if (AP4_FAILED(result)) return result;
    result = stream.WriteString(m_ImageMimeType.GetChars());
    if (AP4_FAILED(result)) return result;
    result = stream.WriteUI08(0);
    if (AP4_FAILED(result)) return result;
    
    return AP4_SUCCESS;
}