AP4_3GppLocalizedStringAtom::AP4_3GppLocalizedStringAtom(Type        type, 
                                                         const char* language, 
                                                         const char* value) :
    AP4_Atom(type, AP4_FULL_ATOM_HEADER_SIZE+2, 0, 0),
    m_Value(value)
{
    m_Language[0] = language[0];
    m_Language[1] = language[1];
    m_Language[2] = language[2];
    m_Language[3] = language[3];
    
    m_Size32 += m_Value.GetLength()+1;
}