node_new_enclose_memory(OnigOptionType option, int is_named)
{
  Node* node = node_new_enclose(ENCLOSE_MEMORY);
  CHECK_NULL_RETURN(node);
  if (is_named != 0)
    SET_ENCLOSE_STATUS(node, NST_NAMED_GROUP);

#ifdef USE_SUBEXP_CALL
  NENCLOSE(node)->option = option;
#endif
  return node;
}