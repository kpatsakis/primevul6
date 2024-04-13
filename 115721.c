AP4_Processor::ProcessFragments(AP4_MoovAtom*              moov, 
                                AP4_List<AP4_AtomLocator>& atoms, 
                                AP4_ContainerAtom*         mfra,
                                AP4_SidxAtom*              sidx,
                                AP4_Position               sidx_position,
                                AP4_ByteStream&            input, 
                                AP4_ByteStream&            output)
{
    unsigned int fragment_index = 0;
    AP4_Array<FragmentMapEntry> fragment_map;
    
    for (AP4_List<AP4_AtomLocator>::Item* item = atoms.FirstItem();
                                          item;
                                          item = item->GetNext(), ++fragment_index) {
        AP4_AtomLocator*   locator     = item->GetData();
        AP4_Atom*          atom        = locator->m_Atom;
        AP4_UI64           atom_offset = locator->m_Offset;
        AP4_UI64           mdat_payload_offset = atom_offset+atom->GetSize()+AP4_ATOM_HEADER_SIZE;
        AP4_Sample         sample;
        AP4_DataBuffer     sample_data_in;
        AP4_DataBuffer     sample_data_out;
        AP4_Result         result;
    
        // if this is not a moof atom, just write it back and continue
        if (atom->GetType() != AP4_ATOM_TYPE_MOOF) {
            result = atom->Write(output);
            if (AP4_FAILED(result)) return result;
            continue;
        }
        
        // parse the moof
        AP4_ContainerAtom* moof = AP4_DYNAMIC_CAST(AP4_ContainerAtom, atom);
        AP4_MovieFragment* fragment = new AP4_MovieFragment(moof);

        // process all the traf atoms
        AP4_Array<AP4_Processor::FragmentHandler*> handlers;
        AP4_Array<AP4_FragmentSampleTable*> sample_tables;
        for (;AP4_Atom* child = moof->GetChild(AP4_ATOM_TYPE_TRAF, handlers.ItemCount());) {
            AP4_ContainerAtom* traf = AP4_DYNAMIC_CAST(AP4_ContainerAtom, child);
            AP4_TfhdAtom* tfhd = AP4_DYNAMIC_CAST(AP4_TfhdAtom, traf->GetChild(AP4_ATOM_TYPE_TFHD));
            
            // find the 'trak' for this track
            AP4_TrakAtom* trak = NULL;
            for (AP4_List<AP4_Atom>::Item* child_item = moov->GetChildren().FirstItem();
                                           child_item;
                                           child_item = child_item->GetNext()) {
                AP4_Atom* child_atom = child_item->GetData();
                if (child_atom->GetType() == AP4_ATOM_TYPE_TRAK) {
                    trak = AP4_DYNAMIC_CAST(AP4_TrakAtom, child_atom);
                    if (trak) {
                        AP4_TkhdAtom* tkhd = AP4_DYNAMIC_CAST(AP4_TkhdAtom, trak->GetChild(AP4_ATOM_TYPE_TKHD));
                        if (tkhd && tkhd->GetTrackId() == tfhd->GetTrackId()) {
                            break;
                        }
                    }
                    trak = NULL;
                }
            }
            
            // find the 'trex' for this track
            AP4_ContainerAtom* mvex = NULL;
            AP4_TrexAtom*      trex = NULL;
            mvex = AP4_DYNAMIC_CAST(AP4_ContainerAtom, moov->GetChild(AP4_ATOM_TYPE_MVEX));
            if (mvex) {
                for (AP4_List<AP4_Atom>::Item* child_item = mvex->GetChildren().FirstItem();
                                               child_item;
                                               child_item = child_item->GetNext()) {
                    AP4_Atom* child_atom = child_item->GetData();
                    if (child_atom->GetType() == AP4_ATOM_TYPE_TREX) {
                        trex = AP4_DYNAMIC_CAST(AP4_TrexAtom, child_atom);
                        if (trex && trex->GetTrackId() == tfhd->GetTrackId()) {
                            break;
                        }
                        trex = NULL;
                    }
                }
            }

            // create the handler for this traf
            AP4_Processor::FragmentHandler* handler = CreateFragmentHandler(trak, trex, traf, input, atom_offset);
            if (handler) {
                result = handler->ProcessFragment();
                if (AP4_FAILED(result)) return result;
            }
            handlers.Append(handler);
            
            // create a sample table object so we can read the sample data
            AP4_FragmentSampleTable* sample_table = NULL;
            result = fragment->CreateSampleTable(moov, tfhd->GetTrackId(), &input, atom_offset, mdat_payload_offset, 0, sample_table);
            if (AP4_FAILED(result)) return result;
            sample_tables.Append(sample_table);
            
            // let the handler look at the samples before we process them
            if (handler) result = handler->PrepareForSamples(sample_table);
            if (AP4_FAILED(result)) return result;
        }
             
        // write the moof
        AP4_UI64 moof_out_start = 0;
        output.Tell(moof_out_start);
        moof->Write(output);
        
        // remember the location of this fragment
        FragmentMapEntry map_entry = {atom_offset, moof_out_start};
        fragment_map.Append(map_entry);

        // write an mdat header
        AP4_Position mdat_out_start;
        AP4_UI64 mdat_size = AP4_ATOM_HEADER_SIZE;
        output.Tell(mdat_out_start);
        output.WriteUI32(0);
        output.WriteUI32(AP4_ATOM_TYPE_MDAT);

        // process all track runs
        for (unsigned int i=0; i<handlers.ItemCount(); i++) {
            AP4_Processor::FragmentHandler* handler = handlers[i];

            // get the track ID
            AP4_ContainerAtom* traf = AP4_DYNAMIC_CAST(AP4_ContainerAtom, moof->GetChild(AP4_ATOM_TYPE_TRAF, i));
            if (traf == NULL) continue;
            AP4_TfhdAtom* tfhd = AP4_DYNAMIC_CAST(AP4_TfhdAtom, traf->GetChild(AP4_ATOM_TYPE_TFHD));
            
            // compute the base data offset
            AP4_UI64 base_data_offset;
            if (tfhd->GetFlags() & AP4_TFHD_FLAG_BASE_DATA_OFFSET_PRESENT) {
                base_data_offset = mdat_out_start+AP4_ATOM_HEADER_SIZE;
            } else {
                base_data_offset = moof_out_start;
            }
            
            // build a list of all trun atoms
            AP4_Array<AP4_TrunAtom*> truns;
            for (AP4_List<AP4_Atom>::Item* child_item = traf->GetChildren().FirstItem();
                                           child_item;
                                           child_item = child_item->GetNext()) {
                AP4_Atom* child_atom = child_item->GetData();
                if (child_atom->GetType() == AP4_ATOM_TYPE_TRUN) {
                    AP4_TrunAtom* trun = AP4_DYNAMIC_CAST(AP4_TrunAtom, child_atom);
                    truns.Append(trun);
                }
            }    
            AP4_Ordinal   trun_index        = 0;
            AP4_Ordinal   trun_sample_index = 0;
            AP4_TrunAtom* trun = truns[0];
            trun->SetDataOffset((AP4_SI32)((mdat_out_start+mdat_size)-base_data_offset));
            
            // write the mdat
            for (unsigned int j=0; j<sample_tables[i]->GetSampleCount(); j++, trun_sample_index++) {
                // advance the trun index if necessary
                if (trun_sample_index >= trun->GetEntries().ItemCount()) {
                    trun = truns[++trun_index];
                    trun->SetDataOffset((AP4_SI32)((mdat_out_start+mdat_size)-base_data_offset));
                    trun_sample_index = 0;
                }
                
                // get the next sample
                result = sample_tables[i]->GetSample(j, sample);
                if (AP4_FAILED(result)) return result;
                sample.ReadData(sample_data_in);
                
                // process the sample data
                if (handler) {
                    result = handler->ProcessSample(sample_data_in, sample_data_out);
                    if (AP4_FAILED(result)) return result;

                    // write the sample data
                    result = output.Write(sample_data_out.GetData(), sample_data_out.GetDataSize());
                    if (AP4_FAILED(result)) return result;

                    // update the mdat size
                    mdat_size += sample_data_out.GetDataSize();
                    
                    // update the trun entry
                    trun->UseEntries()[trun_sample_index].sample_size = sample_data_out.GetDataSize();
                } else {
                    // write the sample data (unmodified)
                    result = output.Write(sample_data_in.GetData(), sample_data_in.GetDataSize());
                    if (AP4_FAILED(result)) return result;

                    // update the mdat size
                    mdat_size += sample_data_in.GetDataSize();
                }
            }

            if (handler) {
                // update the tfhd header
                if (tfhd->GetFlags() & AP4_TFHD_FLAG_BASE_DATA_OFFSET_PRESENT) {
                    tfhd->SetBaseDataOffset(mdat_out_start+AP4_ATOM_HEADER_SIZE);
                }
                if (tfhd->GetFlags() & AP4_TFHD_FLAG_DEFAULT_SAMPLE_SIZE_PRESENT) {
                    tfhd->SetDefaultSampleSize(trun->GetEntries()[0].sample_size);
                }
                
                // give the handler a chance to update the atoms
                handler->FinishFragment();
            }
        }

        // update the mdat header
        AP4_Position mdat_out_end;
        output.Tell(mdat_out_end);
#if defined(AP4_DEBUG)
        AP4_ASSERT(mdat_out_end-mdat_out_start == mdat_size);
#endif
        output.Seek(mdat_out_start);
        output.WriteUI32((AP4_UI32)mdat_size);
        output.Seek(mdat_out_end);
        
        // update the moof if needed
        output.Seek(moof_out_start);
        moof->Write(output);
        output.Seek(mdat_out_end);
        
        // update the sidx if we have one
        if (sidx && fragment_index < sidx->GetReferences().ItemCount()) {
            if (fragment_index == 0) {
                sidx->SetFirstOffset(moof_out_start-(sidx_position+sidx->GetSize()));
            }
            AP4_LargeSize fragment_size = mdat_out_end-moof_out_start;
            AP4_SidxAtom::Reference& sidx_ref = sidx->UseReferences()[fragment_index];
            sidx_ref.m_ReferencedSize = (AP4_UI32)fragment_size;
        }
        
        // cleanup
        delete fragment;
        
        for (unsigned int i=0; i<handlers.ItemCount(); i++) {
            delete handlers[i];
        }
        for (unsigned int i=0; i<sample_tables.ItemCount(); i++) {
            delete sample_tables[i];
        }
    }
    
    // update the mfra if we have one
    if (mfra) {
        for (AP4_List<AP4_Atom>::Item* mfra_item = mfra->GetChildren().FirstItem();
                                       mfra_item;
                                       mfra_item = mfra_item->GetNext()) {
            if (mfra_item->GetData()->GetType() != AP4_ATOM_TYPE_TFRA) continue;
            AP4_TfraAtom* tfra = AP4_DYNAMIC_CAST(AP4_TfraAtom, mfra_item->GetData());
            if (tfra == NULL) continue;
            AP4_Array<AP4_TfraAtom::Entry>& entries     = tfra->GetEntries();
            AP4_Cardinal                    entry_count = entries.ItemCount();
            for (unsigned int i=0; i<entry_count; i++) {
                const FragmentMapEntry* found = FindFragmentMapEntry(fragment_map, entries[i].m_MoofOffset);
                if (found) {
                    entries[i].m_MoofOffset = found->after;
                }
            }
        }
    }
    
    return AP4_SUCCESS;
}