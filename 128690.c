textw_text_release(gs_text_enum_t *pte, client_name_t cname)
{
    textw_text_enum_t *const penum = (textw_text_enum_t *)pte;
    gx_device_txtwrite_t *const tdev = (gx_device_txtwrite_t *) pte->dev;

    /* Free the working buffer where the Unicode was assembled from the enumerated text */
    if (penum->TextBuffer)
        gs_free(tdev->memory, penum->TextBuffer, 1, penum->TextBufferIndex, "txtwrite free temporary text buffer");
    if (penum->Widths)
        gs_free(tdev->memory, penum->Widths, sizeof(float), pte->text.size, "txtwrite free temporary widths array");
    /* If this is copied away when we complete the text enumeration succesfully, then
     * we set the pointer to NULL, if we get here with it non-NULL , then there was
     * an error.
     */
    if (penum->text_state)
        gs_free(tdev->memory, penum->text_state, 1, sizeof(penum->text_state), "txtwrite free text state");

    gs_text_release(pte, cname);
}