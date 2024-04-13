AP_DECLARE(int) ap_is_initial_req(request_rec *r)
{
    return (r->main == NULL)       /* otherwise, this is a sub-request */
           && (r->prev == NULL);   /* otherwise, this is an internal redirect */
}