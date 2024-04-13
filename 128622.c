static inline void tnc_set_sync_state(struct sixpack *sp, int new_tnc_state)
{
	int old_tnc_state = sp->tnc_state;

	if (old_tnc_state != new_tnc_state)
		__tnc_set_sync_state(sp, new_tnc_state);
}