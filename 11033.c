d_lite_rfc3339(VALUE self)
{
    return strftimev("%Y-%m-%dT%H:%M:%S%:z", self, set_tmx);
}