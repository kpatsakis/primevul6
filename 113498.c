AP4_SubtitleSampleEntry::ReadFields(AP4_ByteStream& stream)
{
    // sample entry
    AP4_Result result = AP4_SampleEntry::ReadFields(stream);
    if (result < 0) return result;

    // read fields from this class
    result = stream.ReadNullTerminatedString(m_Namespace);
    if (AP4_FAILED(result)) return result;
    result = stream.ReadNullTerminatedString(m_SchemaLocation);
    if (AP4_FAILED(result)) return result;
    result = stream.ReadNullTerminatedString(m_ImageMimeType);
    if (AP4_FAILED(result)) return result;
    
    return AP4_SUCCESS;
}