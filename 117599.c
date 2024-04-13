    md_unicode_bsearch__(unsigned codepoint, const unsigned* map, size_t map_size)
    {
        int beg, end;
        int pivot_beg, pivot_end;

        beg = 0;
        end = (int) map_size-1;
        while(beg <= end) {
            /* Pivot may be a range, not just a single value. */
            pivot_beg = pivot_end = (beg + end) / 2;
            if(map[pivot_end] & 0x40000000)
                pivot_end++;
            if(map[pivot_beg] & 0x80000000)
                pivot_beg--;

            if(codepoint < (map[pivot_beg] & 0x00ffffff))
                end = pivot_beg - 1;
            else if(codepoint > (map[pivot_end] & 0x00ffffff))
                beg = pivot_end + 1;
            else
                return pivot_beg;
        }

        return -1;
    }