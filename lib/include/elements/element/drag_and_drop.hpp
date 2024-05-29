/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_DRAG_AND_DROP_OCTOBER_11_2023)
#define ELEMENTS_DRAG_AND_DROP_OCTOBER_11_2023

#include <elements/element/proxy.hpp>
#include <elements/element/tracker.hpp>
#include <elements/element/floating.hpp>
#include <elements/element/selection.hpp>
#include <functional>
#include <set>

namespace cycfi { namespace elements
{
   // forward declarations
   class composite_base;

   class drop_base : public proxy_base
   {
   public:

      using base_type = proxy_base;
      using mime_types = std::set<std::string>;

                              drop_base(std::initializer_list<std::string> mime_types_);
      bool                    wants_control() const override;
      void                    prepare_subject(context& ctx) override;
      void                    track_drop(context const& ctx, drop_info const& info, cursor_tracking status) override;
      bool                    drop(context const& ctx, drop_info const& info) override;

      bool                    is_tracking() const     { return _is_tracking; }
      mime_types const&       get_mime_types() const  { return _mime_types; }
      mime_types&             get_mime_types()        { return _mime_types; }

   private:

      bool                    _is_tracking = false;
      mime_types              _mime_types;
   };

   class drop_box_base : public drop_base
   {
   public:

      using base_type = drop_base;
      using on_drop_function = std::function<bool(drop_info const& info)>;

                              drop_box_base(std::initializer_list<std::string> mime_types_);
      void                    draw(context const& ctx) override;
      element*                hit_test(context const& ctx, point p, bool leaf, bool control) override;
      bool                    drop(context const& ctx, drop_info const& info) override;

      on_drop_function        on_drop = [](drop_info const& /*info*/){ return false; };
   };

   template <typename Subject>
   inline proxy<remove_cvref_t<Subject>, drop_box_base>
   drop_box(Subject&& subject, std::initializer_list<std::string> mime_types)
   {
      return {std::forward<Subject>(subject), mime_types};
   }

   class drop_inserter_element : public drop_base
   {
   public:

      using base_type = drop_base;
      using indices_type = std::vector<std::size_t>;
      using on_drop_function = std::function<bool(drop_info const& info, std::size_t ix)>;
      using on_move_function = std::function<void(std::size_t pos, indices_type const& indices)>;
      using on_delete_function = std::function<void(indices_type const& indices)>;
      using on_select_function = std::function<void(indices_type const& indices, std::size_t latest)>;

                              drop_inserter_element(std::initializer_list<std::string> mime_types_);

      void                    draw(context const& ctx) override;
      void                    track_drop(context const& ctx, drop_info const& info, cursor_tracking status) override;
      bool                    drop(context const& ctx, drop_info const& info) override;
      bool                    click(context const& ctx, mouse_button btn) override;
      bool                    key(context const& ctx, key_info k) override;
      bool                    wants_focus() const override { return true; }

      on_drop_function        on_drop = [](drop_info const&, std::size_t){ return false; };
      on_move_function        on_move = [](std::size_t, indices_type const&){};
      on_delete_function      on_erase = [](indices_type const&){};
      on_select_function      on_select = [](indices_type const&, std::size_t){};

      int                     insertion_pos() const { return _insertion_pos; }
      void                    move(indices_type const& indices);
      void                    erase(indices_type const& indices);

   public:

      int                     _insertion_pos = -1;
   };

   namespace detail
   {
      template <typename Subject>
      inline proxy<remove_cvref_t<Subject>, drop_inserter_element>
      make_drop_inserter(Subject&& subject, std::initializer_list<std::string> mime_types)
      {
         return {std::forward<Subject>(subject), mime_types};
      }
   }

   template <typename Subject>
   auto drop_inserter(Subject&& subject, std::initializer_list<std::string> mime_types)
   {
      return
         detail::make_drop_inserter(
            selection_list(std::forward<Subject>(subject))
          , mime_types
         );
   }

   class draggable_element : public tracker<proxy_base>, public selectable
   {
   public:

      view_limits             limits(basic_context const& ctx) const override;
      void                    draw(context const& ctx) override;
      element*                hit_test(context const& ctx, point p, bool leaf, bool control) override;
      bool                    key(context const& ctx, key_info k) override;
      bool                    wants_focus() const override { return true; }

      bool                    is_selected() const override;
      void                    select(bool state) override;

   private:

      void                    begin_tracking(context const& ctx, tracker_info& track_info) override;
      void                    keep_tracking(context const& ctx, tracker_info& track_info) override;
      void                    end_tracking(context const& ctx, tracker_info& track_info) override;

      using drag_image_ptr = std::shared_ptr<floating_element>;

      bool                    _selected = false;
      drag_image_ptr          _drag_image;
   };

   template <typename Subject>
   inline proxy<remove_cvref_t<Subject>, draggable_element>
   draggable(Subject&& subject)
   {
      return {std::forward<Subject>(subject)};
   }

   inline bool draggable_element::is_selected() const
   {
      return _selected;
   }

   inline void draggable_element::select(bool state_)
   {
      _selected = state_;
   }
}}

#endif
