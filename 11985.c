directory_permits_begindir_requests(or_options_t *options)
{
  return options->BridgeRelay != 0 || options->DirPort != 0;
}