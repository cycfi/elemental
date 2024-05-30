/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_MENU_JUNE_4_2016)
#define ELEMENTS_MENU_JUNE_4_2016

#include <elements/element/element.hpp>
#include <elements/element/button.hpp>
#include <elements/element/popup.hpp>
#include <elements/element/selection.hpp>
#include <elements/view.hpp>
#include <infra/support.hpp>

namespace cycfi::elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Basic Dropdown Menu
   ////////////////////////////////////////////////////////////////////////////
   enum class menu_position
   {
      top_left,
      top_right,
      bottom_left,
      bottom_right
   };

   class basic_button_menu : public basic_button
   {
   public:

      using on_open_menu_function = std::function<void(basic_button_menu& menu)>;
      using basic_button::focus;

                              basic_button_menu(menu_position pos = menu_position::bottom_right);

      bool                    click(context const& ctx, mouse_button btn) override;
      void                    drag(context const& ctx, mouse_button btn) override;

      menu_position           position() const              { return _position; }
      void                    position(menu_position pos)   { _position = pos; }

                              template <typename Menu>
      void                    menu(Menu&& menu_);

      on_open_menu_function   on_open_menu;

   private:

      void                    layout_menu(context const& ctx);

      using popup_menu_ptr = std::shared_ptr<basic_popup_menu_element>;

      popup_menu_ptr          _popup;
      menu_position           _position;
   };

   inline basic_button_menu::basic_button_menu(menu_position pos)
    : _position(pos)
   {}

   template <typename Menu>
   inline void basic_button_menu::menu(Menu&& menu_)
   {
      _popup = share(basic_popup_menu(std::forward<Menu>(menu_)));
   }

   ////////////////////////////////////////////////////////////////////////////
   // Menu Items
   ////////////////////////////////////////////////////////////////////////////
   struct shortcut_key
   {
                  shortcut_key(key_code key_, int modifiers_)
                  : key(key_)
                  , modifiers(modifiers_)
                  {
                     if (modifiers & mod_action)
#if defined(__APPLE__)
                        modifiers |= mod_command;
#else
                        modifiers |= mod_control;
#endif
                  }

                  shortcut_key() = default;

      key_code    key = key_code::unknown;
      int         modifiers = 0; // same as modifiers in key_info (see base_view.hpp)
   };

   template <typename Derived>
   struct basic_menu_item_element_base : proxy_base
   {
      bool is_enabled() const override
      {
         return static_cast<Derived const*>(this)->is_enabled();
      }
   };

   class basic_menu_item_element
    : public basic_menu_item_element_base<basic_menu_item_element>
    , public selectable
   {
   public:

      using menu_item_function = std::function<void()>;
      using menu_enabled_function = std::function<bool()>;

      void                    draw(context const& ctx) override;
      element*                hit_test(context const& ctx, point p, bool leaf, bool control) override;
      bool                    click(context const& ctx, mouse_button btn) override;
      bool                    key(context const& ctx, key_info k) override;
      bool                    cursor(context const& ctx, point p, cursor_tracking status) override;
      bool                    wants_control() const override;
      bool                    wants_focus() const override { return true; }

      menu_enabled_function   is_enabled = []{ return true; };
      menu_item_function      on_click;
      shortcut_key            shortcut;

      bool                    is_selected() const override;
      void                    select(bool state) override;

   private:

      bool                    _selected = false;
   };

   template <typename Subject>
   inline proxy<remove_cvref_t<Subject>, basic_menu_item_element>
   basic_menu_item(Subject&& subject)
   {
      return {std::forward<Subject>(subject)};
   }

   inline bool basic_menu_item_element::is_selected() const
   {
      return _selected;
   }

   inline void basic_menu_item_element::select(bool state)
   {
      _selected = state;
   }
}

#endif
