static CURLcode serial_transfers(struct GlobalConfig *global,
                                 CURLSH *share)
{
  CURLcode returncode = CURLE_OK;
  CURLcode result = CURLE_OK;
  struct per_transfer *per;
  bool added = FALSE;

  result = create_transfer(global, share, &added);
  if(result)
    return result;
  if(!added) {
    errorf(global, "no transfer performed\n");
    return CURLE_READ_ERROR;
  }
  for(per = transfers; per;) {
    bool retry;
    long delay;
    bool bailout = FALSE;
    result = pre_transfer(global, per);
    if(result)
      break;

#ifndef CURL_DISABLE_LIBCURL_OPTION
    if(global->libcurl) {
      result = easysrc_perform();
      if(result)
        break;
    }
#endif
#ifdef CURLDEBUG
    if(global->test_event_based)
      result = curl_easy_perform_ev(per->curl);
    else
#endif
      result = curl_easy_perform(per->curl);

    returncode = post_per_transfer(global, per, result, &retry, &delay);
    if(retry) {
      tool_go_sleep(delay);
      continue;
    }

    /* Bail out upon critical errors or --fail-early */
    if(is_fatal_error(returncode) || (returncode && global->fail_early))
      bailout = TRUE;
    else {
      /* setup the next one just before we delete this */
      result = create_transfer(global, share, &added);
      if(result)
        bailout = TRUE;
    }

    per = del_per_transfer(per);

    if(bailout)
      break;
  }
  if(returncode)
    /* returncode errors have priority */
    result = returncode;

  if(result)
    single_transfer_cleanup(global->current);

  return result;
}