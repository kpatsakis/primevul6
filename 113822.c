onig_set_parse_depth_limit(unsigned int depth)
{
  if (depth == 0)
    ParseDepthLimit = DEFAULT_PARSE_DEPTH_LIMIT;
  else
    ParseDepthLimit = depth;
  return 0;
}