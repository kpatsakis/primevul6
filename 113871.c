node_new_option(OnigOptionType option)
{
  Node* node = node_new_enclose(ENCLOSE_OPTION);
  CHECK_NULL_RETURN(node);
  NENCLOSE(node)->option = option;
  return node;
}