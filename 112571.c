AP4_DcfdAtom::AP4_DcfdAtom(AP4_UI32 duration) :
    AP4_Atom(AP4_ATOM_TYPE_DCFD, AP4_FULL_ATOM_HEADER_SIZE+4, 0, 0),
    m_Duration(duration)
{
}