static void http_message_free(ogs_sbi_http_message_t *http)
{
    int i;
    ogs_assert(http);

    if (http->params) {
        ogs_hash_index_t *hi;
        for (hi = ogs_hash_first(http->params); hi; hi = ogs_hash_next(hi)) {
            char *key = (char *)ogs_hash_this_key(hi);
            char *val = ogs_hash_this_val(hi);
            ogs_free(key);
            ogs_free(val);
        }
        ogs_hash_destroy(http->params);
    }

    if (http->headers) {
        ogs_hash_index_t *hi;
        for (hi = ogs_hash_first(http->headers); hi; hi = ogs_hash_next(hi)) {
            char *key = (char *)ogs_hash_this_key(hi);
            char *val = ogs_hash_this_val(hi);
            ogs_free(key);
            ogs_free(val);
        }
        ogs_hash_destroy(http->headers);
    }
    if (http->content)
        ogs_free(http->content);

    for (i = 0; i < http->num_of_part; i++) {
        if (http->part[i].pkbuf)
            ogs_pkbuf_free(http->part[i].pkbuf);
        if (http->part[i].content_id)
            ogs_free(http->part[i].content_id);
        if (http->part[i].content_type)
            ogs_free(http->part[i].content_type);
    }
}