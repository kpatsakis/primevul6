AP_DECLARE(void) ap_hook_check_authz(ap_HOOK_auth_checker_t *pf,
                                     const char * const *aszPre,
                                     const char * const *aszSucc,
                                     int nOrder, int type)
{
    if ((type & AP_AUTH_INTERNAL_MASK) == AP_AUTH_INTERNAL_PER_CONF) {
        ++auth_internal_per_conf_hooks;
    }

    ap_hook_auth_checker(pf, aszPre, aszSucc, nOrder);
}