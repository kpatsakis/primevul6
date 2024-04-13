njs_encode_base64url(njs_str_t *dst, const njs_str_t *src)
{
    njs_encode_base64_core(dst, src, njs_basis64url_enc, 0);
}