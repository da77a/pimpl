#ifndef IMPL_PTR_DETAIL_ONSTACK_HPP
#define IMPL_PTR_DETAIL_ONSTACK_HPP

#include "./traits.hpp"
#include <boost/type_traits/aligned_storage.hpp>

namespace detail
{
    template<typename, size_t> struct onstack;
}

template<typename impl_type, size_t sz>
struct detail::onstack // Proof of concept
{
    struct traits : detail::traits<traits, impl_type>
    {
        void   destroy (impl_type* p) const override { if (p) p->~impl_type(); }
        void construct (void*      to, impl_type const& from) const override { ::new(to) impl_type(from); }
        void    assign (impl_type* to, impl_type const& from) const override { *to = from; }
    };

    using    this_type = onstack;
    using storage_type = typename boost::aligned_storage<sz, 8>::type;

   ~onstack () { if (traits_) traits_->destroy(get()); }
    onstack () {}
    onstack (this_type const& o) : traits_(o.traits_)
    {
        if (traits_)
            traits_->construct(storage_.address(), *o.get());
    }
    this_type& operator=(this_type const& o)
    {
        /**/ if (!traits_ && !o.traits_);
        else if ( traits_ &&  o.traits_) traits_->assign(get(), *o.get());
        else if ( traits_ && !o.traits_) traits_->destroy(get());
        else if (!traits_ &&  o.traits_) o.traits_->construct(storage_.address(), *o.get());

        traits_ = o.traits_;

        return *this;
    }

    template<typename derived_type, typename... arg_types>
    void
    emplace(arg_types&&... args)
    {
        static_assert(sizeof(derived_type) <= sz, "");

        ::new (storage_.address()) derived_type(std::forward<arg_types>(args)...);
        traits_ = traits();
    }
    impl_type* get () const { return traits_ ? (impl_type*) storage_.address() : nullptr; }

    private:

    storage_type         storage_;
    traits_ptr<impl_type> traits_ = nullptr;
};

#endif // IMPL_PTR_DETAIL_ONSTACK_HPP
