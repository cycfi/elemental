/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_LIST_MARCH_2_2020)
#define ELEMENTS_LIST_MARCH_2_2020

#include <elements/element/composite.hpp>
#include <memory>
#include <vector>
#include <functional>
#include <set>
#include<iostream>

namespace cycfi::elements
{
   ////////////////////////////////////////////////////////////////////////////
   // The cell composer abstract class
   ////////////////////////////////////////////////////////////////////////////
   class cell_composer : public std::enable_shared_from_this<cell_composer>
   {
   public:

      struct limits
      {
         float min = 0;
         float max = full_extent;
      };

      virtual std::size_t     size() const = 0;
      virtual void            resize(size_t s) = 0;
      virtual element_ptr     compose(std::size_t index) = 0;
      virtual limits          secondary_axis_limits(basic_context const& ctx) const = 0;
      virtual float           main_axis_size(std::size_t index, basic_context const& ctx) const = 0;
   };

   ////////////////////////////////////////////////////////////////////////////
   // This cell composer has fixed-sized width limits and line-height.
   ////////////////////////////////////////////////////////////////////////////
   template <typename Base = cell_composer>
   class static_limits_cell_composer : public Base
   {
   public:

      using base_type = static_limits_cell_composer<Base>;

                              template <typename... Rest>
                              static_limits_cell_composer(
                                 float min_secondary_axis_size, float main_axis_size
                               , Rest&& ...rest
                              );

                              template <typename... Rest>
                              static_limits_cell_composer(
                                 float min_secondary_axis_size
                               , float max_secondary_axis_size
                               , float main_axis_size
                               , Rest&& ...rest
                              );

      cell_composer::limits   secondary_axis_limits(basic_context const& ctx) const override;
      float                   main_axis_size(std::size_t index, basic_context const& ctx) const override;

   private:

      float                   _main_axis_size;
      cell_composer::limits   _secondary_axis_limits;
   };

   ////////////////////////////////////////////////////////////////////////////
   // This cell composer derives the width limits and line-height from the
   // first element in the list (index 0). This cell composer has fixed-sized
   // width limits and line-height.
   ////////////////////////////////////////////////////////////////////////////
   template <typename Base = cell_composer>
   class fixed_derived_limits_cell_composer : public Base
   {
   public:

      using base_type = fixed_derived_limits_cell_composer<Base>;

                                 template <typename... Rest>
                                 fixed_derived_limits_cell_composer(Rest&& ...rest);

      cell_composer::limits      secondary_axis_limits(basic_context const& ctx) const override;
      float                      main_axis_size(std::size_t index, basic_context const& ctx) const override;

   protected:

      virtual void               get_limits(basic_context const& ctx) const;

      using limits = cell_composer::limits;

      mutable float              _main_axis_size = -1;
      mutable limits             _secondary_axis_limits = {-1, full_extent};
   };

   template<typename Base = cell_composer>
   using vertical_fixed_derived_limits_cell_composer
   [[deprecated("Use vfixed_derived_limits_cell_composer instead.")]]
      = fixed_derived_limits_cell_composer<Base>;

   template<typename Base = cell_composer>
   using vfixed_derived_limits_cell_composer = fixed_derived_limits_cell_composer<Base>;

   template<typename Base = cell_composer>
   class hfixed_derived_limits_cell_composer: public fixed_derived_limits_cell_composer<Base>
   {
   public:

      using base_type = hfixed_derived_limits_cell_composer<Base>;

                                template <typename... Rest>
                                hfixed_derived_limits_cell_composer(Rest&& ...rest);
   protected:

      virtual void              get_limits(basic_context const& ctx) const;
   };

   template<typename Base = cell_composer>
   using horizontal_fixed_derived_limits_cell_composer
   [[deprecated("Use hfixed_derived_limits_cell_composer instead.")]]
      = hfixed_derived_limits_cell_composer<Base>;

   ////////////////////////////////////////////////////////////////////////////
   // This cell composer has fixed-length (number of list elements).
   ////////////////////////////////////////////////////////////////////////////
   template <typename Base = cell_composer>
   class fixed_length_cell_composer : public Base
   {
   public:

      using base_type = fixed_length_cell_composer<Base>;

                              template <typename... Rest>
                              fixed_length_cell_composer(std::size_t size, Rest&& ...rest)
                               : Base(std::forward<Rest>(rest)...)
                               , _size(size)
                              {}

      std::size_t             size() const override { return _size; }
      void                    resize(size_t s) override { _size = s; }

   private:

      std::size_t             _size;
   };

   ////////////////////////////////////////////////////////////////////////////
   // This cell composer composes the cell element using a provided function.
   ////////////////////////////////////////////////////////////////////////////
   template <typename F, typename Base = cell_composer>
   class function_cell_composer : public Base
   {
   public:

      using base_type = function_cell_composer<Base>;

                              template <typename... Rest>
                              function_cell_composer(F&& compose_, Rest&& ...rest)
                               : Base(std::forward<Rest>(rest)...)
                               , _compose(compose_)
                              {}

      virtual element_ptr     compose(std::size_t index) { return _compose(index); }

   private:

      F                       _compose;
   };

   ////////////////////////////////////////////////////////////////////////////
   // basic_cell_composer given the number of elements and a compose function
   ////////////////////////////////////////////////////////////////////////////
   template <typename F>
   inline auto basic_cell_composer(std::size_t size, F&& compose)
   {
      using ftype = remove_cvref_t<F>;
      using return_type =
         vfixed_derived_limits_cell_composer<
            fixed_length_cell_composer<
               function_cell_composer<ftype>
            >
         >;
      return share(return_type{size, std::forward<ftype>(compose)});
   }

   template<typename F>
   [[deprecated("Use basic_vcell_composer instead.")]]
   inline auto basic_vertical_cell_composer(std::size_t size, F&& compose)
   {
      return basic_cell_composer(size, compose);
   }

   template<typename F>
   inline auto basic_vcell_composer(std::size_t size, F&& compose)
   {
      return basic_cell_composer(size, compose);
   }

   template<typename F>
   [[deprecated("Use basic_hcell_composer instead.")]]
   inline auto basic_horizontal_cell_composer(std::size_t size, F&& compose)
   {
      using ftype = remove_cvref_t<F>;
      using return_type =
         hfixed_derived_limits_cell_composer<
            fixed_length_cell_composer<
               function_cell_composer<ftype>
            >
         >;
      return share(return_type{size, std::forward<ftype>(compose)});
   }

   template<typename F>
   inline auto basic_hcell_composer(std::size_t size, F&& compose)
   {
      using ftype = remove_cvref_t<F>;
      using return_type =
         hfixed_derived_limits_cell_composer<
            fixed_length_cell_composer<
               function_cell_composer<ftype>
            >
         >;
      return share(return_type{size, std::forward<ftype>(compose)});
   }

   ////////////////////////////////////////////////////////////////////////////
   // basic_cell_composer given the min_width, line_height, number of
   // elements and a compose function.
   ////////////////////////////////////////////////////////////////////////////
   template <typename F>
   inline auto basic_cell_composer(
      float min_secondary_axis_size, float main_axis_size, std::size_t size, F&& compose
   )
   {
      using ftype = remove_cvref_t<F>;
      using return_type =
         static_limits_cell_composer<
            fixed_length_cell_composer<
               function_cell_composer<ftype>
            >
         >;
      return share(
         return_type{
            min_secondary_axis_size
          , main_axis_size
          , size
          , std::forward<ftype>(compose)
         }
      );
   }

   ////////////////////////////////////////////////////////////////////////////
   // basic_cell_composer given the min_width, max_width, line_height, number
   // of elements and a compose function.
   ////////////////////////////////////////////////////////////////////////////
   template <typename F>
   inline auto basic_cell_composer(
      float min_secondary_axis_size
    , float max_secondary_axis_size
    , float main_axis_size
    , std::size_t size
    , F&& compose
   )
   {
      using ftype = remove_cvref_t<F>;
      using return_type =
         static_limits_cell_composer<
            fixed_length_cell_composer<
               function_cell_composer<ftype>
            >
         >;
      return share(
         return_type{
            min_secondary_axis_size
          , max_secondary_axis_size
          , main_axis_size
          , size
          , std::forward<ftype>(compose)
         }
      );
   }

   ////////////////////////////////////////////////////////////////////////////
   // The main list class -> vertical by default
   ////////////////////////////////////////////////////////////////////////////
   class list : public composite_base
   {
   public:

      using composer_ptr = std::shared_ptr<cell_composer>;
      using indices_type = std::vector<std::size_t>;

                                 list(composer_ptr composer, bool manage_externally = true);
                                 list(list const& rhs);
                                 list(list&& rhs);
      list&                      operator=(list const& rhs);
      list&                      operator=(list&& rhs);

      view_limits                limits(basic_context const& ctx) const override;
      void                       draw(context const& ctx) override;
      void                       layout(context const& ctx) override;

      void                       update();
      void                       clear();
      void                       resize(size_t n);
      bool                       manage_externally() const { return _manage_externally; }
      void                       move(std::size_t pos, indices_type const& indices);
      void                       insert(std::size_t pos, std::size_t num_items);
      void                       erase(indices_type const& indices);

      rect                       bounds_of(context const& ctx, std::size_t ix) const override;

      std::size_t                size() const override;
      element&                   at(std::size_t ix) const override;

      void                       for_each_visible(
                                    context const& ctx
                                  , for_each_callback f
                                  , bool reverse = false
                                 ) const override;

   protected:

      struct cell_info
      {
         double                  pos;
         double                  main_axis_size;
         element_ptr             elem_ptr;
         int                     layout_id = -1;
      };

      // virtual member functions to specialize in hdynamic or vdynamic
      virtual view_limits        make_limits(float main_axis_size, cell_composer::limits secondary_axis_limits ) const;
      virtual float              get_main_axis_start(const rect &r) const;
      virtual float              get_main_axis_end(const rect &r) const;
      virtual void               set_bounds(rect& r, float main_axis_start, cell_info &info) const;
      void                       set_bounds(context& ctx, float main_axis_start, cell_info &info) const;

      using cells_vector = std::vector<cell_info>;
      mutable cells_vector       _cells;

   private:

      void                       sync(basic_context const& ctx) const;
      void                       update(basic_context const& ctx) const;
      void                       move(basic_context const& ctx) const;
      void                       insert(basic_context const& ctx) const;
      void                       erase(basic_context const& ctx) const;

      composer_ptr               _composer;
      bool                       _manage_externally;
      point                      _previous_size;
      std::size_t                _previous_window_start = 0;
      std::size_t                _previous_window_end = 0;

      mutable double             _main_axis_full_size = 0;
      mutable int                _layout_id = 0;

      mutable bool               _update_request:1;
      mutable bool               _move_request:1;
      mutable bool               _insert_request:1;
      mutable bool               _erase_request:1;
      mutable bool               _relinquish_focus_request:1;

      struct request_info
      {
         std::size_t                _move_pos;
         std::vector<std::size_t>   _move_indices;
         std::size_t                _insert_pos;
         std::size_t                _insert_num_items;
         std::vector<std::size_t>   _delete_indices;
      };

      using request_info_ptr = std::unique_ptr<request_info>;
      mutable request_info_ptr      _request_info;
   };

   // The old name is deprecated
   using dynamic_list [[deprecated("Use list instead.")]] = list;

   ////////////////////////////////////////////////////////////////////////////
   // The vertical list class - just an alias
   ////////////////////////////////////////////////////////////////////////////
   using vdynamic_list [[deprecated("Use vlist instead.")]] = list;
   using vlist = list;

   ////////////////////////////////////////////////////////////////////////////
   // The horizontal list class
   ////////////////////////////////////////////////////////////////////////////
   class hlist : public list
   {
   public:
                                 hlist(composer_ptr ptr, bool manage_externally = true)
                                  : list(ptr, manage_externally)
                                 {}
      rect                       bounds_of(context const& ctx, std::size_t ix) const override;

   protected:

      view_limits                make_limits(float main_axis_size, cell_composer::limits secondary_axis_limits) const override;
      void                       set_bounds(rect& r, float main_axis_start, cell_info &info) const override;
      float                      get_main_axis_start(const rect&r) const override;
      float                      get_main_axis_end(const rect &r) const override;
   };

   // The old name is deprecated
   using hdynamic_list [[deprecated("Use hlist instead.")]] = hlist;

   // Utility to move items in a vector `v` from given `indices` to a new position, `pos`.
   template <typename T>
   inline void move_indices(std::vector<T>& v, std::size_t pos, std::vector<std::size_t> const& indices)
   {
      // Precondition: The indices should be validly pointing to items in vector `v`.

      // Reserve space for moved elements
      std::vector<T> to_move;
      to_move.reserve(indices.size());

      // Create a copy of the elements to be moved and erase the elements
      // from their original positions. Iterating in reverse, after this
      // operation, `to_move` will contain the items in reverse order.
      for (auto i = indices.crbegin(); i != indices.crend(); ++i)
      {
         to_move.push_back(std::move(v[*i]));
         v.erase(v.begin()+*i);
         if (pos > *i)
            --pos;
      }

      // Determine the insert position
      auto pos_i = v.begin() + std::min(pos, v.size());

      // Insert the elements at the new position in reverse order
      v.insert(pos_i, to_move.crbegin(), to_move.crend());
   }

   // Utility to erase items in a vector `v` with given `indices`.
   template <typename T>
   inline void erase_indices(std::vector<T>& v, std::vector<std::size_t> const& indices)
   {
      // Precondition: The indices should be validly pointing to items in vector `v`.
      for (auto i = indices.crbegin(); i != indices.crend(); ++i)
         v.erase(v.begin()+*i);
   }

   ////////////////////////////////////////////////////////////////////////////
   // Inlines
   ////////////////////////////////////////////////////////////////////////////
   template <typename Base>
   template <typename... Rest>
   inline static_limits_cell_composer<Base>::static_limits_cell_composer(
      float min_secondary_axis_size
    , float main_axis_size
    , Rest&& ...rest
   )
    : Base(std::forward<Rest>(rest)...)
    , _main_axis_size{main_axis_size}
    , _secondary_axis_limits{min_secondary_axis_size, full_extent}
   {}

   template <typename Base>
   template <typename... Rest>
   inline static_limits_cell_composer<Base>::static_limits_cell_composer(
      float min_secondary_axis_size
    , float max_secondary_axis_size
    , float main_axis_size
    , Rest&& ...rest
   )
    : Base(std::forward<Rest>(rest)...)
    , _main_axis_size(main_axis_size)
    , _secondary_axis_limits{min_secondary_axis_size, max_secondary_axis_size}
   {}

   template <typename Base>
   inline cell_composer::limits
   static_limits_cell_composer<Base>::secondary_axis_limits(basic_context const& /*ctx*/) const
   {
      return _secondary_axis_limits;
   }

   template <typename Base>
   inline float static_limits_cell_composer<Base>::main_axis_size(std::size_t /*index*/, basic_context const& /*ctx*/) const
   {
      return _main_axis_size;
   }

   ////////////////////////////////////////////////////////////////////////////
   template <typename Base>
   template <typename... Rest>
   inline fixed_derived_limits_cell_composer<Base>::fixed_derived_limits_cell_composer(
      Rest&& ...rest
   )
    : Base(std::forward<Rest>(rest)...)
    , _main_axis_size{-1}
    , _secondary_axis_limits{-1, full_extent}
   {}

   template <typename Base>
   inline cell_composer::limits
   fixed_derived_limits_cell_composer<Base>::secondary_axis_limits(basic_context const& ctx) const
   {
      if (_secondary_axis_limits.min == -1)
         get_limits(ctx);
      return _secondary_axis_limits;
   }

   template <typename Base>
   inline float fixed_derived_limits_cell_composer<Base>::main_axis_size(std::size_t /*index*/, basic_context const& ctx) const
   {
      if (_main_axis_size == -1)
         get_limits(ctx);
      return _main_axis_size;
   }

   template <typename Base>
   void fixed_derived_limits_cell_composer<Base>::get_limits(basic_context const& ctx) const
   {
      auto lim = const_cast<base_type*>(this)->compose(0)->limits(ctx);
      _secondary_axis_limits.min = lim.min.x;
      _secondary_axis_limits.max = lim.max.x;
      _main_axis_size = lim.min.y;
   }

   template <typename Base>
   template <typename... Rest>
   inline hfixed_derived_limits_cell_composer<Base>::hfixed_derived_limits_cell_composer(
      Rest&& ...rest
   )
    : fixed_derived_limits_cell_composer<Base> (std::forward<Rest>(rest)...)
   {}

   template<typename Base>
   void hfixed_derived_limits_cell_composer<Base>::get_limits(basic_context const& ctx)  const
   {
      auto lim = const_cast<base_type*>(this)->compose(0)->limits(ctx);
      this->_secondary_axis_limits.min = lim.min.y;
      this->_secondary_axis_limits.max = lim.max.y;
      this->_main_axis_size = lim.min.x;
   }

   inline void list::set_bounds(context& ctx, float main_axis_pos, cell_info &cell) const
   {
      set_bounds(ctx.bounds, main_axis_pos, cell);
   }
}

#endif
