uc_err uc_hook_del(uc_engine *uc, uc_hook hh)
{
    int i;
    struct hook *hook = (struct hook *)hh;

    UC_INIT(uc);

    // we can't dereference hook->type if hook is invalid
    // so for now we need to iterate over all possible types to remove the hook
    // which is less efficient
    // an optimization would be to align the hook pointer
    // and store the type mask in the hook pointer.
    for (i = 0; i < UC_HOOK_MAX; i++) {
        if (list_exists(&uc->hook[i], (void *)hook)) {
            hook->to_delete = true;
            uc->hooks_count[i]--;
            list_append(&uc->hooks_to_del, hook);
        }
    }

    return UC_ERR_OK;
}