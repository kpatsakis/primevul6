AP_CORE_DECLARE_NONSTD(apr_status_t) ap_sub_req_output_filter(ap_filter_t *f,
                                                              apr_bucket_brigade *bb)
{
    apr_bucket *e = APR_BRIGADE_LAST(bb);

    if (APR_BUCKET_IS_EOS(e)) {
        apr_bucket_delete(e);
    }

    if (!APR_BRIGADE_EMPTY(bb)) {
        return ap_pass_brigade(f->next, bb);
    }

    return APR_SUCCESS;
}