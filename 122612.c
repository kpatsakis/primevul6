njs_primitive_values_compare(njs_vm_t *vm, njs_value_t *val1, njs_value_t *val2)
{
    double   num1, num2;

    if (njs_fast_path(njs_is_numeric(val1))) {
        num1 = njs_number(val1);

        if (njs_fast_path(njs_is_numeric(val2))) {
            num2 = njs_number(val2);

        } else {
            num2 = njs_string_to_number(val2, 0);
        }

    } else if (njs_is_numeric(val2)) {
        num1 = njs_string_to_number(val1, 0);
        num2 = njs_number(val2);

    } else {
        return (njs_string_cmp(val1, val2) < 0) ? 1 : 0;
    }

    /* NaN and void values are not comparable with anything. */
    if (isnan(num1) || isnan(num2)) {
        return -1;
    }

    /* Infinities are handled correctly by comparision. */
    return (num1 < num2);
}