adds_flags (guint32 old_flags, guint32 new_flags)
{
  return (new_flags & ~old_flags) != 0;
}