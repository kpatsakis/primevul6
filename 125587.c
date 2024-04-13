stream *streamNew(void) {
    stream *s = zmalloc(sizeof(*s));
    s->rax = raxNew();
    s->length = 0;
    s->first_id.ms = 0;
    s->first_id.seq = 0;
    s->last_id.ms = 0;
    s->last_id.seq = 0;
    s->max_deleted_entry_id.seq = 0;
    s->max_deleted_entry_id.ms = 0;
    s->entries_added = 0;
    s->cgroups = NULL; /* Created on demand to save memory when not used. */
    return s;
}