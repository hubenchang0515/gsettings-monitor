#include <stdio.h>
#include "gsettings_blob.h"

/***************************************************************
 * @brief 解析blob数据，定位各个部分的首地址和长度
 * @param[in] blob 完整的blob数据
 * @param[in] len blob数据的长度（字节）
 * @return 使用blob构建出的GVariant，失败时为NULL，需要手动释放
 * *************************************************************/
GVariant* gsettings_blob_parse(const void* blob, size_t len)
{
    GVariant* v = g_variant_new_from_data(G_VARIANT_TYPE("a{smv}"), blob, len, true, NULL, NULL);
    return v;
}