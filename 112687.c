AP4_AtomFactory::CreateAtomsFromStream(AP4_ByteStream& stream, 
                                       AP4_AtomParent& atoms)
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
    return CreateAtomsFromStream(stream, bytes_available, atoms);
}