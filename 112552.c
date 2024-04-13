AP4_DcfdAtom::WriteFields(AP4_ByteStream& stream)
{
    stream.WriteUI32(m_Duration);
    return AP4_SUCCESS;
}