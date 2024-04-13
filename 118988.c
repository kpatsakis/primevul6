AP_DECLARE(int) ap_some_auth_required(request_rec *r)
{
    /* Is there a require line configured for the type of *this* req? */
    if (ap__authz_ap_some_auth_required) {
        return ap__authz_ap_some_auth_required(r);
    }
    else
        return 0;
}