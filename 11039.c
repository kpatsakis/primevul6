d_lite_to_s(VALUE self)
{
    return strftimev("%Y-%m-%d", self, set_tmx);
}