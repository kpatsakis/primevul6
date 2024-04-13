static CURLcode run_all_transfers(struct GlobalConfig *global,
                                  CURLSH *share,
                                  CURLcode result)
{
  /* Save the values of noprogress and isatty to restore them later on */
  bool orig_noprogress = global->noprogress;
  bool orig_isatty = global->isatty;
  struct per_transfer *per;

  /* Time to actually do the transfers */
  if(!result) {
    if(global->parallel)
      result = parallel_transfers(global, share);
    else
      result = serial_transfers(global, share);
  }

  /* cleanup if there are any left */
  for(per = transfers; per;) {
    bool retry;
    long delay;
    CURLcode result2 = post_per_transfer(global, per, result, &retry, &delay);
    if(!result)
      /* don't overwrite the original error */
      result = result2;

    /* Free list of given URLs */
    clean_getout(per->config);

    per = del_per_transfer(per);
  }

  /* Reset the global config variables */
  global->noprogress = orig_noprogress;
  global->isatty = orig_isatty;


  return result;
}