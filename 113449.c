AP4_AtomSampleTable::GetNearestSyncSampleIndex(AP4_Ordinal sample_index, bool before)
{
    // if we don't have an stss table, all samples match
    if (m_StssAtom == NULL) return sample_index;
    
    sample_index += 1; // the table is 1-based
    AP4_Cardinal entry_count = m_StssAtom->GetEntries().ItemCount();
    if (before) {
        AP4_Ordinal cursor = 0;    
        for (unsigned int i=0; i<entry_count; i++) {
            if (m_StssAtom->GetEntries()[i] >= sample_index) return cursor;
            if (m_StssAtom->GetEntries()[i]) cursor = m_StssAtom->GetEntries()[i]-1;
        }

        // not found?
        return cursor;
    } else {
        for (unsigned int i=0; i<entry_count; i++) {
            if (m_StssAtom->GetEntries()[i] >= sample_index) {
                return m_StssAtom->GetEntries()[i]?m_StssAtom->GetEntries()[i]-1:sample_index-1;
            }
        }

        // not found?
        return GetSampleCount();
    }
}