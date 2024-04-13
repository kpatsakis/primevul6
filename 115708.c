AP4_AvccAtom::WriteFields(AP4_ByteStream& stream)
{
    return stream.Write(m_RawBytes.GetData(), m_RawBytes.GetDataSize());
}