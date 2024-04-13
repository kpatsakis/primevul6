AP4_DataAtom::WriteFields(AP4_ByteStream& stream)
{
    stream.WriteUI32(m_DataType);
    stream.WriteUI32(m_DataLang);
    if (m_Source) {
        AP4_LargeSize size = 0;
        m_Source->GetSize(size);
        m_Source->Seek(0);
        m_Source->CopyTo(stream, size);
    }

    return AP4_SUCCESS;
}