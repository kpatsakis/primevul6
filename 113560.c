AP4_StszAtom::Create(AP4_Size size, AP4_ByteStream& stream)
{
    AP4_UI08 version;
    AP4_UI32 flags;
    if (AP4_FAILED(AP4_Atom::ReadFullHeader(stream, version, flags))) return NULL;
    if (version != 0) return NULL;
    return new AP4_StszAtom(size, version, flags, stream);
}