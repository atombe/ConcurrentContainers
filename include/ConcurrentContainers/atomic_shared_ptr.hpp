/*

Copyright (c) 2016, Adam Byerly (atombe)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1.  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

2.  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

3.  All advertising materials mentioning features or use of this
    software must display the following acknowledgement:
    This product includes software developed by Adam Byerly.

4.  The name of Adam Byerly may not be used to endorse or promote products
    derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY ADAM BYERLY "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL ADAM BYERLY BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#ifndef ATOMBE_ATOMIC_SHARED_PTR_HPP
#define ATOMBE_ATOMIC_SHARED_PTR_HPP

#include <cstddef>
#include <atomic>
#include <utility>

namespace atombe {
namespace util {

template<
    typename T
> atomic_shared_ptr {
  public:
    typedef T value_type;

    atomic_shared_ptr()
        : ptr_{nullptr} { }

    atomic_shared_ptr(std::nullptr_t)
        : ptr_{nullptr} { }

    template< typename Y >
    explicit atomic_shared_ptr(Y* ptr)
        : ptr_{ptr} { }

    // Atomic types are neither CopyConstructable nore CopyAssignable
    /******************************************************************/
    atomic_shared_ptr(const AtomicSharedPtr& ptr) = delete;
    template< typename Y >
    atomic_shared_ptr(const AtomicSharedPtr<Y>& ptr) = delete;
    atomic_shared_ptr& operator=(const AtomicSharedPtr&) = delete;
    template< typename Y >
    atomic_shared_ptr<Y>& operator=(const AtomicSharedPtr<Y>&) = delete;
    /******************************************************************/

    atomic_shared_ptr& operator=(AtomicSharedPtr desired)
    {
        store(desired);
    }

    bool is_lock_free() const
    {
        return std::atomic_is_lock_free(ptr_);
    }

    atomic_shared_ptr load(
        std::memory_order order = std::memory_order_seq_cst) const
    {
        return std::atomic_load_explicit(ptr_, order);
    }

    void store(
        atomic_shared_ptr desired,
        std::memory_order order = std::memory_order_seq_cst)
    {
        std::atomic_store_explicit(desired.ptr_, order);
    }

    atomic_shared_ptr exchange(
        AtomicSharedPtr desired,
        std::memory_order order = std::memory_order_seq_cst)
    {
        return std::atomic_exchange_explicit(ptr_,
                                             desired.ptr_,
                                             order);
    }

    bool compare_exchange_weak(
        atomic_shared_ptr expected,
        AtomicSharedPtr desired)
    {
        return std::atomic_compare_exchange_weak(ptr_,
                                                 expected.ptr_,
                                                 desired._ptr);
    }

    bool compare_exchange_strong(
        atomic_shared_ptr expected,
        AtomicSharedPtr desired)
    {
        return std::atomic_compare_exchange_strong(ptr_,
                                                   expected.ptr_,
                                                   desired._ptr);
    }

  private:
    std::shared_ptr<value_type> ptr_;
};

}   // namespace util
}   // namespace atombe

#endif  // ATOMBE_ATOMIC_SHARED_PTR_HPP
