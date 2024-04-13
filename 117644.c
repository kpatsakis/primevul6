md_free_ref_defs(MD_CTX* ctx)
{
    int i;

    for(i = 0; i < ctx->n_ref_defs; i++) {
        MD_REF_DEF* def = &ctx->ref_defs[i];

        if(def->label_needs_free)
            free(def->label);
        if(def->title_needs_free)
            free(def->title);
    }

    free(ctx->ref_defs);
}