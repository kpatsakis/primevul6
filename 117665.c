static CURLcode create_transfer(struct GlobalConfig *global,
                                CURLSH *share,
                                bool *added)
{
  CURLcode result = CURLE_OK;
  *added = FALSE;
  while(global->current) {
    result = transfer_per_config(global, global->current, share, added);
    if(!result && !*added) {
      /* when one set is drained, continue to next */
      global->current = global->current->next;
      continue;
    }
    break;
  }
  return result;
}