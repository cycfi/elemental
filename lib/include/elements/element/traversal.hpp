/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_TRAVERSAL_MAY_23_2020)
#define ELEMENTS_TRAVERSAL_MAY_23_2020

#include <elements/element/proxy.hpp>
#include <elements/element/indirect.hpp>
#include <elements/element/composite.hpp>
#include <type_traits>

namespace cycfi::elements
{
   namespace detail
   {
      template <typename Ptr>
      inline Ptr find_element_impl(element* e_)
      {
         if (auto* e = dynamic_cast<Ptr>(e_))
            return e;

         if (auto* e = dynamic_cast<indirect_base*>(e_))
            return find_element_impl<Ptr>(&e->get());

         return nullptr;
      }

      template <typename Ptr>
      inline Ptr find_element_impl(element const* e_)
      {
         if (auto* e = dynamic_cast<Ptr>(e_))
            return e;

         if (auto* e = dynamic_cast<indirect_base const*>(e_))
            return find_element_impl<Ptr>(&e->get());

         return nullptr;
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   // find_subject utility finds the outermost subject with the given pointer
   // type or nullptr if not found. Searches subjects of proxies only.
   ////////////////////////////////////////////////////////////////////////////
   template <typename Ptr>
   inline Ptr find_subject(element* e_)
   {
      proxy_base* proxy = dynamic_cast<proxy_base*>(e_);
      while (proxy)
      {
         auto* subject = &proxy->subject();
         if (auto* e = detail::find_element_impl<Ptr>(subject))
            return e;
         proxy = dynamic_cast<proxy_base*>(subject);
      }
      return nullptr;
   }

   template <typename Ptr>
   inline Ptr find_subject(element const* e_)
   {
      proxy_base const* proxy = dynamic_cast<proxy_base const*>(e_);
      while (proxy)
      {
         auto* subject = &proxy->subject();
         if (auto* e = detail::find_element_impl<Ptr>(subject))
            return e;
         proxy = dynamic_cast<proxy_base const*>(subject);
      }
      return nullptr;
   }

   ////////////////////////////////////////////////////////////////////////////
   // find_element utility finds the outermost subject with the given pointer
   // type or nullptr if not found. Searches an element and if it is a proxy,
   // searches its subject too.
   ////////////////////////////////////////////////////////////////////////////
   template <typename Ptr>
   inline Ptr find_element(element* e_)
   {
      if (auto* e = detail::find_element_impl<Ptr>(e_))
         return e;
      return find_subject<Ptr>(e_);
   }

   ////////////////////////////////////////////////////////////////////////////
   // find_composite utility finds the innermost composite given a context.
   // If successful, returns a pointer to the composite base and pointer
   // to its context.
   ////////////////////////////////////////////////////////////////////////////
   std::pair<composite_base*, context const*>
   inline find_composite(context const& ctx)
   {
      element* this_ = ctx.element;
      std::pair<composite_base*, context const*> result = {nullptr, nullptr};
      auto p = ctx.parent;
      while (p)
      {
         auto&& find =
            [&](context const& ctx, element* e) -> bool
            {
               if (auto c = dynamic_cast<composite_base*>(e); c && c != this_)
               {
                  result.first = c;
                  result.second = &ctx;
                  return true;
               }
               return false;
            };

         auto e = p->element;
         if (find(*p, e))
            return result;

         proxy_base* proxy = dynamic_cast<proxy_base*>(e);
         while (proxy)
         {
            auto* subject = &proxy->subject();
            if (find(*p, subject))
               return result;
            proxy = dynamic_cast<proxy_base*>(subject);
         }
         if (auto* indirect = dynamic_cast<indirect_base*>(e))
         {
            auto* subject = &indirect->get();
            if (find(*p, subject))
               return result;
         }
         p = p->parent;
      }
      return result;
   }

   ////////////////////////////////////////////////////////////////////////////
   // find_parent utility finds the innermost parent element with the given
   // pointer type or nullptr if not found, given a context. If successful,
   // returns a pointer to the parent element.
   ////////////////////////////////////////////////////////////////////////////
   template <typename Ptr>
   inline Ptr find_parent(context const& ctx)
   {
      auto p = ctx.parent;
      while (p)
      {
         if (auto* e = detail::find_element_impl<Ptr>(p->element))
            return e;
         p = p->parent;
      }
      return nullptr;
   }

   ////////////////////////////////////////////////////////////////////////////
   // find_parent_context utility finds the innermost parent context with an
   // element of a given pointer type or nullptr if not found, given a
   // context. If successful, returns a pointer to the parent context.
   ////////////////////////////////////////////////////////////////////////////
   template <typename Ptr>
   inline context const* find_parent_context(context const& ctx)
   {
      auto p = ctx.parent;
      while (p)
      {
         if (auto* e = detail::find_element_impl<Ptr>(p->element))
            return p;
         p = p->parent;
      }
      return nullptr;
   }
}

#endif
