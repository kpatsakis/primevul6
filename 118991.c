AP_DECLARE(request_rec *) ap_sub_req_lookup_uri(const char *new_uri,
                                                const request_rec *r,
                                                ap_filter_t *next_filter)
{
    return ap_sub_req_method_uri("GET", new_uri, r, next_filter);
}