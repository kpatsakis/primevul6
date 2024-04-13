variableLength *MAPIFindProperty(MAPIProps *p, unsigned int ID) {
  int i;
  if (p != NULL) {
    for (i = 0; i < p->count; i++) {
      if ((p->properties[i].id == ID) && (p->properties[i].custom == 0)) {
        return (p->properties[i].data);
      }
    }
  }
  return MAPI_UNDEFINED;
}