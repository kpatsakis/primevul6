static void single_transfer_cleanup(struct OperationConfig *config)
{
  if(config) {
    struct State *state = &config->state;
    if(state->urls) {
      /* Free list of remaining URLs */
      glob_cleanup(state->urls);
      state->urls = NULL;
    }
    Curl_safefree(state->outfiles);
    Curl_safefree(state->httpgetfields);
    Curl_safefree(state->uploadfile);
    if(state->inglob) {
      /* Free list of globbed upload files */
      glob_cleanup(state->inglob);
      state->inglob = NULL;
    }
  }
}