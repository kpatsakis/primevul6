AP4_SampleEntry::Write(AP4_ByteStream& stream)
{
    AP4_Result result;

    // write the header
    result = WriteHeader(stream);
    if (AP4_FAILED(result)) return result;

    // write the fields
    result = WriteFields(stream);
    if (AP4_FAILED(result)) return result;

    // write the children atoms
    return m_Children.Apply(AP4_AtomListWriter(stream));
}