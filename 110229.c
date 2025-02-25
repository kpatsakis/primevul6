lys_restr_dup(struct lys_module *mod, struct lys_restr *old, int size, int shallow, struct unres_schema *unres)
{
    struct lys_restr *result;
    int i;

    if (!size) {
        return NULL;
    }

    result = calloc(size, sizeof *result);
    LY_CHECK_ERR_RETURN(!result, LOGMEM(mod->ctx), NULL);

    for (i = 0; i < size; i++) {
        result[i].ext_size = old[i].ext_size;
        lys_ext_dup(mod->ctx, mod, old[i].ext, old[i].ext_size, &result[i], LYEXT_PAR_RESTR, &result[i].ext, shallow, unres);
        result[i].expr = lydict_insert(mod->ctx, old[i].expr, 0);
        result[i].dsc = lydict_insert(mod->ctx, old[i].dsc, 0);
        result[i].ref = lydict_insert(mod->ctx, old[i].ref, 0);
        result[i].eapptag = lydict_insert(mod->ctx, old[i].eapptag, 0);
        result[i].emsg = lydict_insert(mod->ctx, old[i].emsg, 0);
    }

    return result;
}