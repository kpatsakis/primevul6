coolkey_make_new_id(unsigned long object_id, unsigned long id_type)
{ return ((object_id  & 0x00ffffffUL)|(id_type << 24)); }