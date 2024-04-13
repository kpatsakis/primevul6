md_ascii_case_eq(const CHAR* s1, const CHAR* s2, SZ n)
{
    OFF i;
    for(i = 0; i < n; i++) {
        CHAR ch1 = s1[i];
        CHAR ch2 = s2[i];

        if(ISLOWER_(ch1))
            ch1 += ('A'-'a');
        if(ISLOWER_(ch2))
            ch2 += ('A'-'a');
        if(ch1 != ch2)
            return FALSE;
    }
    return TRUE;
}