md_free_ref_def_hashtable(MD_CTX* ctx)
{
    if(ctx->ref_def_hashtable != NULL) {
        int i;

        for(i = 0; i < ctx->ref_def_hashtable_size; i++) {
            void* bucket = ctx->ref_def_hashtable[i];
            if(bucket == NULL)
                continue;
            if(ctx->ref_defs <= (MD_REF_DEF*) bucket  &&  (MD_REF_DEF*) bucket < ctx->ref_defs + ctx->n_ref_defs)
                continue;
            free(bucket);
        }

        free(ctx->ref_def_hashtable);
    }
}