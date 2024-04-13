md_process_verbatim_block_contents(MD_CTX* ctx, MD_TEXTTYPE text_type, const MD_VERBATIMLINE* lines, int n_lines)
{
    static const CHAR indent_chunk_str[] = _T("                ");
    static const SZ indent_chunk_size = SIZEOF_ARRAY(indent_chunk_str) - 1;

    int i;
    int ret = 0;

    for(i = 0; i < n_lines; i++) {
        const MD_VERBATIMLINE* line = &lines[i];
        int indent = line->indent;

        MD_ASSERT(indent >= 0);

        /* Output code indentation. */
        while(indent > (int) indent_chunk_size) {
            MD_TEXT(text_type, indent_chunk_str, indent_chunk_size);
            indent -= indent_chunk_size;
        }
        if(indent > 0)
            MD_TEXT(text_type, indent_chunk_str, indent);

        /* Output the code line itself. */
        MD_TEXT_INSECURE(text_type, STR(line->beg), line->end - line->beg);

        /* Enforce end-of-line. */
        MD_TEXT(text_type, _T("\n"), 1);
    }

abort:
    return ret;
}