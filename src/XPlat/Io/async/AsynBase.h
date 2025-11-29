#ifndef XPLAT_IO_ASYNC_ASYNCBASE_H
#define XPLAT_IO_ASYNC_ASYNCBASE_H

#include <XPlat/Function.h>

namespace xplat
{

    class AsyncIOOp;
    class IoUringOp;

    /**
     * An AsyncBaseOp represents a pending operation.
     * Set a notification callback or using this class methods directly.
     * The op must remain allocated until it is completed or canceled.
    */

    class AsyncBaseOp    
    {
    public:
        AsyncBaseOp();
        ~AsyncBaseOp();
    };

} //namespace xplat

#endif // XPLAT_IO_ASYNC_ASYNCBASE_H