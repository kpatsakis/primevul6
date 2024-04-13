AP4_SampleEntry::Read(AP4_ByteStream& stream, AP4_AtomFactory& atom_factory)
{
    // read the fields before the children atoms
    ReadFields(stream);

    // read children atoms (ex: esds and maybe others)
    // NOTE: not all sample entries have children atoms
    AP4_Size payload_size = (AP4_Size)(GetSize()-GetHeaderSize());
    AP4_Size fields_size = GetFieldsSize();
    if (payload_size > fields_size) {
        ReadChildren(atom_factory, stream, payload_size-fields_size);
    }
}