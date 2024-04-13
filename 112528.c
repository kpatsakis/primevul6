AP4_MetaData::Entry::AddToFileDcf(AP4_File& file, AP4_Ordinal index)
{
    // check that we have a correct entry
    if (m_Value == NULL) return AP4_ERROR_INVALID_STATE;
    
    // look for 'odrm/odhe'
    AP4_ContainerAtom* odhe = AP4_DYNAMIC_CAST(AP4_ContainerAtom, file.FindChild("odrm/odhe"));
    if (odhe == NULL) return AP4_ERROR_NO_SUCH_ITEM;

    // get/create the list of entries
    AP4_ContainerAtom* udta = AP4_DYNAMIC_CAST(AP4_ContainerAtom, odhe->FindChild("udta", true));
    if (udta == NULL) return AP4_ERROR_INTERNAL;
    
    // convert the entry into an atom
    AP4_Atom* data_atom;
    AP4_Result result = ToAtom(data_atom);
    if (AP4_FAILED(result)) return result;

    // add the entry's data to the container
    return udta->AddChild(data_atom, index);
}