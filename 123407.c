AP4_HvccAtom::WriteFields(AP4_ByteStream& stream)
{
    return stream.Write(m_RawBytes.GetData(), m_RawBytes.GetDataSize());
}