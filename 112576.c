AP4_DcfdAtom::AP4_DcfdAtom(AP4_UI08        version,
                           AP4_UI32        flags,
                           AP4_ByteStream& stream) :
    AP4_Atom(AP4_ATOM_TYPE_DCFD, AP4_FULL_ATOM_HEADER_SIZE+4, version, flags),
    m_Duration(0)
{
    stream.ReadUI32(m_Duration);
}