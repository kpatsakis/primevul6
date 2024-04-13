AP4_SampleEntry::ReadFields(AP4_ByteStream& stream)
{
    stream.Read(m_Reserved1, sizeof(m_Reserved1));
    stream.ReadUI16(m_DataReferenceIndex);

    return AP4_SUCCESS;
}