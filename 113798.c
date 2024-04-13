is_invalid_quantifier_target(Node* node)
{
  switch (NTYPE(node)) {
  case NT_ANCHOR:
    return 1;
    break;

  case NT_ENCLOSE:
    /* allow enclosed elements */
    /* return is_invalid_quantifier_target(NENCLOSE(node)->target); */
    break;

  case NT_LIST:
    do {
      if (! is_invalid_quantifier_target(NCAR(node))) return 0;
    } while (IS_NOT_NULL(node = NCDR(node)));
    return 0;
    break;

  case NT_ALT:
    do {
      if (is_invalid_quantifier_target(NCAR(node))) return 1;
    } while (IS_NOT_NULL(node = NCDR(node)));
    break;

  default:
    break;
  }
  return 0;
}