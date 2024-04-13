AP4_MetaDataStringAtom::AP4_MetaDataStringAtom(Type type, AP4_UI32 size, AP4_ByteStream& stream) :
    AP4_Atom(type, size),
    m_Reserved(0),
    m_Value((AP4_Size)(size-AP4_ATOM_HEADER_SIZE-4))
{
    stream.ReadUI32(m_Reserved);
    stream.Read(m_Value.UseChars(), m_Value.GetLength());
}