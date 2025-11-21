//
// Created by 76426 on 2025/11/16.
//

#ifndef XPLAN_FILE_H
#define XPLAN_FILE_H

#include <string>
#include "System/Standard.h"

namespace xplat
{

    typedef std::string String;
    class XPLAT_API File
    {

    public:
        File() = default;
        File(const String &absolutePath);
        File(const File &);
        ~File() = default;
        File &operator=(const String &newAbsolutePath);

    private:
        //==============================================================================
        String fullPath_;
    };

} // xplat

#endif // XPLAN_FILE_H
