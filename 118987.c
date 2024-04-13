AP_DECLARE(void) ap_hook_check_authn(ap_HOOK_check_user_id_t *pf,
                                     const char * const *aszPre,
                                     const char * const *aszSucc,
                                     int nOrder, int type)
{
    if ((type & AP_AUTH_INTERNAL_MASK) == AP_AUTH_INTERNAL_PER_CONF) {
        ++auth_internal_per_conf_hooks;
    }

    ap_hook_check_user_id(pf, aszPre, aszSucc, nOrder);
}