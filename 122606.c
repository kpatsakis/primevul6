njs_string_concat(njs_vm_t *vm, njs_value_t *val1, njs_value_t *val2)
{
    u_char             *start;
    size_t             size, length;
    njs_string_prop_t  string1, string2;

    (void) njs_string_prop(&string1, val1);
    (void) njs_string_prop(&string2, val2);

    /*
     * A result of concatenation of Byte and ASCII or UTF-8 strings
     * is a Byte string.
     */
    if ((string1.length != 0 || string1.size == 0)
        && (string2.length != 0 || string2.size == 0))
    {
        length = string1.length + string2.length;

    } else {
        length = 0;
    }

    size = string1.size + string2.size;

    start = njs_string_alloc(vm, &vm->retval, size, length);

    if (njs_slow_path(start == NULL)) {
        return NJS_ERROR;
    }

    (void) memcpy(start, string1.start, string1.size);
    (void) memcpy(start + string1.size, string2.start, string2.size);

    return sizeof(njs_vmcode_3addr_t);
}