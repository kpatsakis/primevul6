AP4_DcfdAtom::Create(AP4_UI32 size, AP4_ByteStream& stream) 
{
    AP4_UI08 version;
    AP4_UI32 flags;
    if (AP4_FAILED(AP4_Atom::ReadFullHeader(stream, version, flags))) return NULL;
    if (version != 0) return NULL;
    if (size != AP4_FULL_ATOM_HEADER_SIZE+4) return NULL;
    return new AP4_DcfdAtom(version, flags, stream);
}