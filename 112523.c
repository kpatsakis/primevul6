AP4_DcfStringAtom::AP4_DcfStringAtom(Type type, const char* value) :
    AP4_Atom(type, AP4_FULL_ATOM_HEADER_SIZE, 0, 0),
    m_Value(value)
{
    m_Size32 += m_Value.GetLength();
}