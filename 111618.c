static void clear_deleted_hooks(uc_engine *uc)
{
    struct list_item *cur;
    struct hook *hook;
    int i;

    for (cur = uc->hooks_to_del.head;
         cur != NULL && (hook = (struct hook *)cur->data); cur = cur->next) {
        assert(hook->to_delete);
        for (i = 0; i < UC_HOOK_MAX; i++) {
            if (list_remove(&uc->hook[i], (void *)hook)) {
                if (--hook->refs == 0) {
                    uc->del_inline_hook(uc, hook);
                    free(hook);
                }

                // a hook cannot be twice in the same list
                break;
            }
        }
    }

    list_clear(&uc->hooks_to_del);
}