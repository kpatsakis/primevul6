AP4_MetaData::ParseUdta(AP4_ContainerAtom* udta, const char* namespc)
{
    // check that the atom is indeed a 'udta' atom
    if (udta->GetType() != AP4_ATOM_TYPE_UDTA) {
        return AP4_ERROR_INVALID_PARAMETERS;
    }
    
    AP4_List<AP4_Atom>::Item* udta_item = udta->GetChildren().FirstItem();
    for (; udta_item; udta_item = udta_item->GetNext()) {
        AP4_3GppLocalizedStringAtom* _3gpp_atom = AP4_DYNAMIC_CAST(AP4_3GppLocalizedStringAtom, udta_item->GetData()); 
        if (_3gpp_atom) {
            Add3GppEntry(_3gpp_atom, namespc);
            continue;
        } 
        
        AP4_DcfStringAtom* dcfs_atom = AP4_DYNAMIC_CAST(AP4_DcfStringAtom, udta_item->GetData());
        if (dcfs_atom) {
            AddDcfStringEntry(dcfs_atom, namespc);
            continue;
        } 

        AP4_DcfdAtom* dcfd_atom = AP4_DYNAMIC_CAST(AP4_DcfdAtom, udta_item->GetData());
        if (dcfd_atom) {
            AddDcfdEntry(dcfd_atom, namespc);
        }
    }
    
    return AP4_SUCCESS;
}