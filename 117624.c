md_lookup_ref_def(MD_CTX* ctx, const CHAR* label, SZ label_size)
{
    unsigned hash;
    void* bucket;

    if(ctx->ref_def_hashtable_size == 0)
        return NULL;

    hash = md_link_label_hash(label, label_size);
    bucket = ctx->ref_def_hashtable[hash % ctx->ref_def_hashtable_size];

    if(bucket == NULL) {
        return NULL;
    } else if(ctx->ref_defs <= (MD_REF_DEF*) bucket  &&  (MD_REF_DEF*) bucket < ctx->ref_defs + ctx->n_ref_defs) {
        const MD_REF_DEF* def = (MD_REF_DEF*) bucket;

        if(md_link_label_cmp(def->label, def->label_size, label, label_size) == 0)
            return def;
        else
            return NULL;
    } else {
        MD_REF_DEF_LIST* list = (MD_REF_DEF_LIST*) bucket;
        MD_REF_DEF key_buf;
        const MD_REF_DEF* key = &key_buf;
        const MD_REF_DEF** ret;

        key_buf.label = (CHAR*) label;
        key_buf.label_size = label_size;
        key_buf.hash = md_link_label_hash(key_buf.label, key_buf.label_size);

        ret = (const MD_REF_DEF**) bsearch(&key, list->ref_defs,
                    list->n_ref_defs, sizeof(MD_REF_DEF*), md_ref_def_cmp);
        if(ret != NULL)
            return *ret;
        else
            return NULL;
    }
}