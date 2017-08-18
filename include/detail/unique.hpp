#ifndef IMPL_PTR_DETAIL_UNIQUE_HPP
#define IMPL_PTR_DETAIL_UNIQUE_HPP

#include "./traits.hpp"
#include "./is_allocator.hpp"

namespace detail
{
    template<typename> struct unique;
}

template<typename impl_type>
struct detail::unique
{
    struct traits : detail::traits<traits, impl_type>
    {
        void destroy(impl_type* p) const override { boost::checked_delete(p); }
    };

    // Smart-pointer with the unique-value-semantics behavior.

    using this_type = unique;

//  template<typename derived_type, typename alloc_type, typename... arg_types>
//  typename std::enable_if<is_allocator<alloc_type>::value, void>::type
//  emplace(alloc_type&& alloc, arg_types&&... args)
//  {
//      void* mem = std::allocator_traits<alloc_type>::allocate(alloc, 1);
//      reset(new(mem) derived_type(std::forward<arg_types>(args)...));
//  }
    template<typename derived_type, typename... arg_types>
    typename std::enable_if<!is_allocator<typename first<arg_types...>::type>::value, void>::type
    emplace(arg_types&&... args)
    {
        impl_type* impl = new derived_type(std::forward<arg_types>(args)...);

        this_type(impl).swap(*this);
    }

   ~unique () { if (traits_) traits_->destroy(impl_); }
    unique () {}
    unique (impl_type* p) : impl_(p), traits_(traits()) {}

    unique (this_type&& o) { swap(o); }
    this_type& operator= (this_type&& o) { swap(o); return *this; }

    unique (this_type const&) =delete;
    this_type& operator= (this_type const&) =delete;

    bool operator< (this_type const& o) const { return impl_ < o.impl_; }
    void      swap (this_type& o) { std::swap(impl_, o.impl_), std::swap(traits_, o.traits_); }
    impl_type* get () const { return impl_; }
    long use_count () const { return 1; }

    private:

    impl_type*              impl_ = nullptr;
    traits_ptr<impl_type> traits_ = nullptr;
};

#endif // IMPL_PTR_DETAIL_UNIQUE_HPP
