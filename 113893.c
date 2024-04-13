scan_env_set_mem_node(ScanEnv* env, int num, Node* node)
{
  if (env->num_mem >= num)
    SCANENV_MEM_NODES(env)[num] = node;
  else
    return ONIGERR_PARSER_BUG;
  return 0;
}