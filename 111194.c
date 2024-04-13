loop_break(codegen_scope *s, node *tree)
{
  if (!s->loop) {
    codegen(s, tree, NOVAL);
    raise_error(s, "unexpected break");
  }
  else {
    struct loopinfo *loop;


    loop = s->loop;
    if (tree) {
      if (loop->reg < 0) {
        codegen(s, tree, NOVAL);
      }
      else {
        gen_retval(s, tree);
      }
    }
    while (loop) {
      if (loop->type == LOOP_BEGIN) {
        loop = loop->prev;
      }
      else if (loop->type == LOOP_RESCUE) {
        loop = loop->prev;
      }
      else{
        break;
      }
    }
    if (!loop) {
      raise_error(s, "unexpected break");
      return;
    }

    if (loop->type == LOOP_NORMAL) {
      int tmp;

      if (loop->reg >= 0) {
        if (tree) {
          gen_move(s, loop->reg, cursp(), 0);
        }
        else {
          genop_1(s, OP_LOADNIL, loop->reg);
        }
      }
      tmp = genjmp(s, OP_JMPUW, loop->pc2);
      loop->pc2 = tmp;
    }
    else {
      if (!tree) {
        genop_1(s, OP_LOADNIL, cursp());
      }
      gen_return(s, OP_BREAK, cursp());
    }
  }
}