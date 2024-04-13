static MngBox mng_read_box(MngBox previous_box,char delta_type,
  unsigned char *p)
{
   MngBox
      box;

  /*
    Read clipping boundaries from DEFI, CLIP, FRAM, or PAST chunk.
  */
  box.left=(ssize_t) ((p[0]  << 24) | (p[1]  << 16) | (p[2]  << 8) | p[3]);
  box.right=(ssize_t) ((p[4]  << 24) | (p[5]  << 16) | (p[6]  << 8) | p[7]);
  box.top=(ssize_t) ((p[8]  << 24) | (p[9]  << 16) | (p[10] << 8) | p[11]);
  box.bottom=(ssize_t) ((p[12] << 24) | (p[13] << 16) | (p[14] << 8) | p[15]);
  if (delta_type != 0)
    {
      box.left+=previous_box.left;
      box.right+=previous_box.right;
      box.top+=previous_box.top;
      box.bottom+=previous_box.bottom;
    }

  return(box);
}