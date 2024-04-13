static size_t session_estimate_headers_payload(nghttp2_session *session,
                                               const nghttp2_nv *nva,
                                               size_t nvlen,
                                               size_t additional) {
  return nghttp2_hd_deflate_bound(&session->hd_deflater, nva, nvlen) +
         additional;
}