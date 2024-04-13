static long url_proto(char *url)
{
  CURLU *uh = curl_url();
  long proto = 0;
  if(uh) {
    if(url) {
      if(!curl_url_set(uh, CURLUPART_URL, url,
                       CURLU_GUESS_SCHEME | CURLU_NON_SUPPORT_SCHEME)) {
        char *schemep = NULL;
        if(!curl_url_get(uh, CURLUPART_SCHEME, &schemep,
                         CURLU_DEFAULT_SCHEME) &&
           schemep) {
          proto = scheme2protocol(schemep);
          curl_free(schemep);
        }
      }
    }
    curl_url_cleanup(uh);
  }
  return proto;
}