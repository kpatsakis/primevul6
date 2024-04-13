AP_DECLARE(void) ap_destroy_sub_req(request_rec *r)
{
    /* Reclaim the space */
    apr_pool_destroy(r->pool);
}