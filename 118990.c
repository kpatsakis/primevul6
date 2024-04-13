static int decl_die(int status, const char *phase, request_rec *r)
{
    if (status == DECLINED) {
        ap_log_rerror(APLOG_MARK, APLOG_CRIT, 0, r, APLOGNO(00025)
                      "configuration error:  couldn't %s: %s", phase, r->uri);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    else {
        ap_log_rerror(APLOG_MARK, APLOG_TRACE3, 0, r,
                      "auth phase '%s' gave status %d: %s", phase,
                      status, r->uri);
        return status;
    }
}