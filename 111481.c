void _WM_Release_Allowance(struct _mdi *mdi) {
    uint32_t release = 0;
    uint32_t longest_release = 0;
    
    struct _note *note = mdi->note;
    
    while (note != NULL) {
        
        if (note->modes & SAMPLE_ENVELOPE) {
            //ensure envelope isin a release state
            if (note->env < 4) {
                note->env = 4;
            }
        
            // make sure this is set
            note->env_inc = -note->sample->env_rate[note->env];
        
            release = note->env_level / -note->env_inc;
        } else {
            // Sample release
            if (note->modes & SAMPLE_LOOP) {
                note->modes ^= SAMPLE_LOOP;
            }
            release = note->sample->data_length - note->sample_pos;
        }
        
        if (release > longest_release) longest_release = release;
        note->replay = NULL;
        note = note->next;
    }
    
    mdi->samples_to_mix = longest_release;
    
    return;
}