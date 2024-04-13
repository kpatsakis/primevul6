static char Lookahead(unsigned char *word, int how_far) {
  char letter_ahead = '\0';  /* null by default */
  int idx;
  for (idx = 0; word[idx] != '\0' && idx < how_far; idx++);
  /* Edge forward in the string... */

  letter_ahead = (char)word[idx];  /* idx will be either == to how_far or
                                    * at the end of the string
                                    */
  return letter_ahead;
}