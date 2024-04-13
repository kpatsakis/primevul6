pop_colours(uint ix)
{
  if (ix > 10)
    return;

  if (ix) {  // retrieve
    colours_cur = ix;
    ix--;
  }
  else {  // pop
    if (colours_cur) {
      colours_cur--;
      ix = colours_cur;
    }
    else
      return;
  }
  //printf("pop %d\n", ix);

  if (colours_stack[ix])
    memcpy(colours, colours_stack[ix], COLOUR_NUM * sizeof(COLORREF));
}