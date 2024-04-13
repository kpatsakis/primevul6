AP4_MetaDataStringAtom::AP4_MetaDataStringAtom(Type type, const char* value) :
    AP4_Atom(type, AP4_ATOM_HEADER_SIZE),
    m_Reserved(0),
    m_Value(value)
{
    m_Size32 += 4+m_Value.GetLength();
}