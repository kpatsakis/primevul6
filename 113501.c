AP4_SubtitleSampleEntry::AP4_SubtitleSampleEntry(
    AP4_Atom::Type    format,
    const char*       namespce,
    const char*       schema_location,
    const char*       image_mime_type) :
    AP4_SampleEntry(format),
    m_Namespace(namespce),
    m_SchemaLocation(schema_location),
    m_ImageMimeType(image_mime_type)
{
    SetSize(m_Size32+m_Namespace.GetLength()+1+m_SchemaLocation.GetLength()+1+m_ImageMimeType.GetLength()+1);
}