static CURLcode parallel_transfers(struct GlobalConfig *global,
                                   CURLSH *share)
{
  CURLM *multi;
  CURLMcode mcode = CURLM_OK;
  CURLcode result = CURLE_OK;
  int still_running = 1;
  struct timeval start = tvnow();
  bool more_transfers;
  bool added_transfers;
  /* wrapitup is set TRUE after a critical error occurs to end all transfers */
  bool wrapitup = FALSE;
  /* wrapitup_processed is set TRUE after the per transfer abort flag is set */
  bool wrapitup_processed = FALSE;
  time_t tick = time(NULL);

  multi = curl_multi_init();
  if(!multi)
    return CURLE_OUT_OF_MEMORY;

  result = add_parallel_transfers(global, multi, share,
                                  &more_transfers, &added_transfers);
  if(result) {
    curl_multi_cleanup(multi);
    return result;
  }

  while(!mcode && (still_running || more_transfers)) {
    /* If stopping prematurely (eg due to a --fail-early condition) then signal
       that any transfers in the multi should abort (via progress callback). */
    if(wrapitup) {
      if(!still_running)
        break;
      if(!wrapitup_processed) {
        struct per_transfer *per;
        for(per = transfers; per; per = per->next) {
          if(per->added)
            per->abort = TRUE;
        }
        wrapitup_processed = TRUE;
      }
    }

    mcode = curl_multi_poll(multi, NULL, 0, 1000, NULL);
    if(!mcode)
      mcode = curl_multi_perform(multi, &still_running);

    progress_meter(global, &start, FALSE);

    if(!mcode) {
      int rc;
      CURLMsg *msg;
      bool checkmore = FALSE;
      do {
        msg = curl_multi_info_read(multi, &rc);
        if(msg) {
          bool retry;
          long delay;
          struct per_transfer *ended;
          CURL *easy = msg->easy_handle;
          CURLcode tres = msg->data.result;
          curl_easy_getinfo(easy, CURLINFO_PRIVATE, (void *)&ended);
          curl_multi_remove_handle(multi, easy);

          if(ended->abort && tres == CURLE_ABORTED_BY_CALLBACK) {
            msnprintf(ended->errorbuffer, sizeof(ended->errorbuffer),
              "Transfer aborted due to critical error in another transfer");
          }
          tres = post_per_transfer(global, ended, tres, &retry, &delay);
          progress_finalize(ended); /* before it goes away */
          all_added--; /* one fewer added */
          checkmore = TRUE;
          if(retry) {
            ended->added = FALSE; /* add it again */
            /* we delay retries in full integer seconds only */
            ended->startat = delay ? time(NULL) + delay/1000 : 0;
          }
          else {
            /* result receives this transfer's error unless the transfer was
               marked for abort due to a critical error in another transfer */
            if(tres && (!ended->abort || !result))
              result = tres;
            if(is_fatal_error(result) || (result && global->fail_early))
              wrapitup = TRUE;
            (void)del_per_transfer(ended);
          }
        }
      } while(msg);
      if(wrapitup) {
        if(still_running)
          continue;
        else
          break;
      }
      if(!checkmore) {
        time_t tock = time(NULL);
        if(tick != tock) {
          checkmore = TRUE;
          tick = tock;
        }
      }
      if(checkmore) {
        /* one or more transfers completed, add more! */
        CURLcode tres = add_parallel_transfers(global, multi, share,
                                               &more_transfers,
                                               &added_transfers);
        if(tres)
          result = tres;
        if(added_transfers)
          /* we added new ones, make sure the loop doesn't exit yet */
          still_running = 1;
      }
      if(is_fatal_error(result) || (result && global->fail_early))
        wrapitup = TRUE;
    }
  }

  (void)progress_meter(global, &start, TRUE);

  /* Make sure to return some kind of error if there was a multi problem */
  if(mcode) {
    result = (mcode == CURLM_OUT_OF_MEMORY) ? CURLE_OUT_OF_MEMORY :
      /* The other multi errors should never happen, so return
         something suitably generic */
      CURLE_BAD_FUNCTION_ARGUMENT;
  }

  curl_multi_cleanup(multi);

  return result;
}