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

#include <atomic>
#include <memory>
#include <utility>

#include <ConcurrentContainers/atomic_shared_ptr.hpp>

template<
    typename T
> class concurrent_list
{
  public:
    typedef T value_type;

    concurrent_list() = default;
    ~concurrent_list() = default;

    // TODO
    class ref
    {
        std::shared_ptr<node> ptr_;
      public:
        ref(std::shared_ptr<node> ptr)
            : ptr_{ptr} { }
        value_type& operator*() { return ptr_->data_; }
        value_type* operator->() { return &ptr_->data_; }
    };

    ref find(value_type data) const
    {
        auto ptr = std::atomic_load(&head_);
        while (ptr && ptr->data_ != data)
            ptr = ptr->next_;
        return ref( std::move(ptr) )
    }

    void push_front(value_type data)
    {
        auto n = std::make_shared<node>();
        n->data_ = data;
        n->next_ = std::atomic_load(head_);
        while (!std::atomic_compare_exchange_weak(&head_, &n->next_, n)) { }
    }

  protected:
    concurrent_list(const ConcurrentList& other) = delete;
    concurrent_list& operator=(const ConcurrentList& rhs) = delete;

    struct node
    {
        value_type data_,
        std::shared_ptr<node> next_;
    };

    std::shared_ptr<node> head_{nullptr};
}
