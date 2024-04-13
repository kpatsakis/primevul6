static CURLcode transfer_per_config(struct GlobalConfig *global,
                                    struct OperationConfig *config,
                                    CURLSH *share,
                                    bool *added)
{
  CURLcode result = CURLE_OK;
  bool capath_from_env;
  *added = FALSE;

  /* Check we have a url */
  if(!config->url_list || !config->url_list->url) {
    helpf(global->errors, "no URL specified!\n");
    return CURLE_FAILED_INIT;
  }

  /* On WIN32 we can't set the path to curl-ca-bundle.crt
   * at compile time. So we look here for the file in two ways:
   * 1: look at the environment variable CURL_CA_BUNDLE for a path
   * 2: if #1 isn't found, use the windows API function SearchPath()
   *    to find it along the app's path (includes app's dir and CWD)
   *
   * We support the environment variable thing for non-Windows platforms
   * too. Just for the sake of it.
   */
  capath_from_env = false;
  if(!config->cacert &&
     !config->capath &&
     (!config->insecure_ok || (config->doh_url && !config->doh_insecure_ok))) {
    CURL *curltls = curl_easy_init();
    struct curl_tlssessioninfo *tls_backend_info = NULL;

    /* With the addition of CAINFO support for Schannel, this search could find
     * a certificate bundle that was previously ignored. To maintain backward
     * compatibility, only perform this search if not using Schannel.
     */
    result = curl_easy_getinfo(curltls, CURLINFO_TLS_SSL_PTR,
                               &tls_backend_info);
    if(result)
      return result;

    /* Set the CA cert locations specified in the environment. For Windows if
     * no environment-specified filename is found then check for CA bundle
     * default filename curl-ca-bundle.crt in the user's PATH.
     *
     * If Schannel is the selected SSL backend then these locations are
     * ignored. We allow setting CA location for schannel only when explicitly
     * specified by the user via CURLOPT_CAINFO / --cacert.
     */
    if(tls_backend_info->backend != CURLSSLBACKEND_SCHANNEL) {
      char *env;
      env = curlx_getenv("CURL_CA_BUNDLE");
      if(env) {
        config->cacert = strdup(env);
        if(!config->cacert) {
          curl_free(env);
          errorf(global, "out of memory\n");
          return CURLE_OUT_OF_MEMORY;
        }
      }
      else {
        env = curlx_getenv("SSL_CERT_DIR");
        if(env) {
          config->capath = strdup(env);
          if(!config->capath) {
            curl_free(env);
            helpf(global->errors, "out of memory\n");
            return CURLE_OUT_OF_MEMORY;
          }
          capath_from_env = true;
        }
        else {
          env = curlx_getenv("SSL_CERT_FILE");
          if(env) {
            config->cacert = strdup(env);
            if(!config->cacert) {
              curl_free(env);
              errorf(global, "out of memory\n");
              return CURLE_OUT_OF_MEMORY;
            }
          }
        }
      }

      if(env)
        curl_free(env);
#ifdef WIN32
      else {
        result = FindWin32CACert(config, tls_backend_info->backend,
                                 TEXT("curl-ca-bundle.crt"));
      }
#endif
    }
    curl_easy_cleanup(curltls);
  }

  if(!result)
    result = single_transfer(global, config, share, capath_from_env, added);

  return result;
}