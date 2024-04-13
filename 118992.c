AP_DECLARE(void) ap_setup_auth_internal(apr_pool_t *ptemp)
{
    int total_auth_hooks = 0;
    int total_auth_providers = 0;

    auth_internal_per_conf = 0;

    if (_hooks.link_access_checker) {
        total_auth_hooks += _hooks.link_access_checker->nelts;
    }
    if (_hooks.link_access_checker_ex) {
        total_auth_hooks += _hooks.link_access_checker_ex->nelts;
    }
    if (_hooks.link_check_user_id) {
        total_auth_hooks += _hooks.link_check_user_id->nelts;
    }
    if (_hooks.link_auth_checker) {
        total_auth_hooks += _hooks.link_auth_checker->nelts;
    }

    if (total_auth_hooks > auth_internal_per_conf_hooks) {
        return;
    }

    total_auth_providers +=
        ap_list_provider_names(ptemp, AUTHN_PROVIDER_GROUP,
                               AUTHN_PROVIDER_VERSION)->nelts;
    total_auth_providers +=
        ap_list_provider_names(ptemp, AUTHZ_PROVIDER_GROUP,
                               AUTHZ_PROVIDER_VERSION)->nelts;

    if (total_auth_providers > auth_internal_per_conf_providers) {
        return;
    }

    auth_internal_per_conf = 1;
}