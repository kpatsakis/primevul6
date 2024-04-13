AP4_AtomFactory::CreateAtomFromStream(AP4_ByteStream& stream, 
                                      AP4_Atom*&      atom)
{
    AP4_LargeSize stream_size     = 0;
    AP4_Position  stream_position = 0;
    AP4_LargeSize bytes_available = (AP4_LargeSize)(-1);
    if (AP4_SUCCEEDED(stream.GetSize(stream_size)) && 
        stream_size != 0 &&
        AP4_SUCCEEDED(stream.Tell(stream_position)) &&
        stream_position <= stream_size) {
        bytes_available = stream_size-stream_position;
    }
    return CreateAtomFromStream(stream, bytes_available, atom);
}