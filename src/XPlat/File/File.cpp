//
// Created by 76426 on 2025/11/16.
//

#include "File.h"

namespace xplat
{
    File::File(const String &fullPathName)
        : fullPath_(fullPathName)
    {
    }

    File::File(const File &other)
        : fullPath_(other.fullPath_)
    {
    }

    File &File::operator=(const String &newAbsolutePath)
    {
        fullPath_ = newAbsolutePath;
        return *this;
    }

} // namespace xplat
