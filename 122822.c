void TNEFFreeMapiProps(MAPIProps *p) {
  int i, j;
  for (i = 0; i < p->count; i++) {
    for (j = 0; j < p->properties[i].count; j++) {
      FREEVARLENGTH(p->properties[i].data[j]);
    }
    free(p->properties[i].data);
    for (j = 0; j < p->properties[i].namedproperty; j++) {
      FREEVARLENGTH(p->properties[i].propnames[j]);
    }
    free(p->properties[i].propnames);
  }
  free(p->properties);
  p->count = 0;
}