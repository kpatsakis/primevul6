AP4_DcfStringAtom::WriteFields(AP4_ByteStream& stream)
{
    if (m_Value.GetLength()) stream.Write(m_Value.GetChars(), m_Value.GetLength());
    return AP4_SUCCESS;
}