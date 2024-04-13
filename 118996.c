AP_DECLARE(int) ap_some_authn_required(request_rec *r)
{
    int access_status;

    switch (ap_satisfies(r)) {
    case SATISFY_ALL:
    case SATISFY_NOSPEC:
        if ((access_status = ap_run_access_checker(r)) != OK) {
            break;
        }

        access_status = ap_run_access_checker_ex(r);
        if (access_status == DECLINED) {
            return TRUE;
        }

        break;
    case SATISFY_ANY:
        if ((access_status = ap_run_access_checker(r)) == OK) {
            break;
        }

        access_status = ap_run_access_checker_ex(r);
        if (access_status == DECLINED) {
            return TRUE;
        }

        break;
    }

    return FALSE;
}