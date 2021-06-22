#ifndef GSETTINGS_BLOB_H
#define GSETTINGS_BLOB_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <glib.h>

/***************************************************************
 * @brief 解析blob数据，定位各个部分的首地址和长度
 * @param[in] blob 完整的blob数据
 * @param[in] len blob数据的长度（字节）
 * @return 使用blob构建出的GVariant，失败时为NULL
 * *************************************************************/
GVariant* gsettings_blob_parse(const void* blob, size_t len);

#endif // GSETTINGS_BLOB_H