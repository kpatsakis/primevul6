static void address_space_dispatch_free(AddressSpaceDispatch *d)
{
    phys_sections_free(&d->map);
    g_free(d);
}