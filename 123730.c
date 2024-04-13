fill_bitmap_hash(ASS_Renderer *priv, GlyphInfo *info,
                 OutlineBitmapHashKey *hash_key)
{
    hash_key->frx = rot_key(info->frx);
    hash_key->fry = rot_key(info->fry);
    hash_key->frz = rot_key(info->frz);
    hash_key->fax = double_to_d16(info->fax);
    hash_key->fay = double_to_d16(info->fay);
}