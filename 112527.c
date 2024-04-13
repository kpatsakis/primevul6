AP4_MetaData::AP4_MetaData(AP4_File* file)
{
    // get the file's movie
    AP4_Movie* movie = file->GetMovie();

    // handle the movie's metadata if there is a movie in the file
    if (movie) {
        AP4_MoovAtom* moov = movie->GetMoovAtom();
        if (moov == NULL) return;
        ParseMoov(moov);
        AP4_Atom* udta = moov->GetChild(AP4_ATOM_TYPE_UDTA);
        if (udta) {
            AP4_ContainerAtom* udta_container = AP4_DYNAMIC_CAST(AP4_ContainerAtom, udta);
            if (udta_container) {
                ParseUdta(udta_container, "3gpp");
            }
        }
    } else {
        // if we don't have a movie, try to show metadata from a udta atom
        AP4_List<AP4_Atom>& top_level_atoms = file->GetTopLevelAtoms();
        
        AP4_List<AP4_Atom>::Item* atom_item = top_level_atoms.FirstItem();
        while (atom_item) {
            AP4_ContainerAtom* container = AP4_DYNAMIC_CAST(AP4_ContainerAtom, atom_item->GetData());
            if (container) {
                // look for a udta in a DCF layout
                AP4_Atom* udta = container->FindChild("odhe/udta");
                if (udta) {
                    AP4_ContainerAtom* udta_container = AP4_DYNAMIC_CAST(AP4_ContainerAtom, udta);
                    if (udta_container) {
                        ParseUdta(udta_container, "dcf");
                    }
                }
            }
            atom_item = atom_item->GetNext();
        }
    } 
}