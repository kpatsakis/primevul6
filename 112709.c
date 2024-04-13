AP4_HdlrAtom::AP4_HdlrAtom(AP4_Atom::Type hdlr_type, const char* hdlr_name) :
    AP4_Atom(AP4_ATOM_TYPE_HDLR, AP4_FULL_ATOM_HEADER_SIZE, 0, 0),
    m_HandlerType(hdlr_type),
    m_HandlerName(hdlr_name)
{
    m_Size32 += 20+m_HandlerName.GetLength()+1;
    m_Reserved[0] = m_Reserved[1] = m_Reserved[2] = 0;
}