AP4_AtomFactory::CreateAtomsFromStream(AP4_ByteStream& stream, 
                                       AP4_LargeSize   bytes_available,
                                       AP4_AtomParent& atoms)
{
    AP4_Result result;
    do {
        AP4_Atom* atom = NULL;
        result = CreateAtomFromStream(stream, bytes_available, atom);
        if (AP4_SUCCEEDED(result) && atom != NULL) {
            atoms.AddChild(atom);
        }
    } while (AP4_SUCCEEDED(result));
    
    return AP4_SUCCESS;
}