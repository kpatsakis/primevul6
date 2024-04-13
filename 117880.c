bool unit_name_is_valid(const char *n, UnitNameFlags flags) {
        const char *e, *i, *at;

        assert((flags & ~(UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE)) == 0);

        if (_unlikely_(flags == 0))
                return false;

        if (isempty(n))
                return false;

        if (strlen(n) >= UNIT_NAME_MAX)
                return false;

        e = strrchr(n, '.');
        if (!e || e == n)
                return false;

        if (unit_type_from_string(e + 1) < 0)
                return false;

        for (i = n, at = NULL; i < e; i++) {

                if (*i == '@' && !at)
                        at = i;

                if (!strchr("@" VALID_CHARS, *i))
                        return false;
        }

        if (at == n)
                return false;

        if (flags & UNIT_NAME_PLAIN)
                if (!at)
                        return true;

        if (flags & UNIT_NAME_INSTANCE)
                if (at && e > at + 1)
                        return true;

        if (flags & UNIT_NAME_TEMPLATE)
                if (at && e == at + 1)
                        return true;

        return false;
}