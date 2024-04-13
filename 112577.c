AP4_MetaDataStringAtom::WriteFields(AP4_ByteStream& stream)
{
    stream.WriteUI32(m_Reserved);
    return stream.Write(m_Value.GetChars(), m_Value.GetLength());
}