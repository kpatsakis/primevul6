gen_literal_array(codegen_scope *s, node *tree, mrb_bool sym, int val)
{
  if (val) {
    int i = 0, j = 0, gen = 0;

    while (tree) {
      switch (nint(tree->car->car)) {
      case NODE_STR:
        if ((tree->cdr == NULL) && (nint(tree->car->cdr->cdr) == 0))
          break;
        /* fall through */
      case NODE_BEGIN:
        codegen(s, tree->car, VAL);
        ++j;
        break;

      case NODE_LITERAL_DELIM:
        if (j > 0) {
          j = 0;
          ++i;
          if (sym)
            gen_intern(s);
        }
        break;
      }
      while (j >= 2) {
        pop(); pop();
        genop_1(s, OP_STRCAT, cursp());
        push();
        j--;
      }
      if (i > GEN_LIT_ARY_MAX) {
        pop_n(i);
        if (gen) {
          pop();
          genop_2(s, OP_ARYPUSH, cursp(), i);
        }
        else {
          genop_2(s, OP_ARRAY, cursp(), i);
          gen = 1;
        }
        push();
        i = 0;
      }
      tree = tree->cdr;
    }
    if (j > 0) {
      ++i;
      if (sym)
        gen_intern(s);
    }
    pop_n(i);
    if (gen) {
      pop();
      genop_2(s, OP_ARYPUSH, cursp(), i);
    }
    else {
      genop_2(s, OP_ARRAY, cursp(), i);
    }
    push();
  }
  else {
    while (tree) {
      switch (nint(tree->car->car)) {
      case NODE_BEGIN: case NODE_BLOCK:
        codegen(s, tree->car, NOVAL);
      }
      tree = tree->cdr;
    }
  }
}