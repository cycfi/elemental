/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_GALLERY_ICON_BUTTON_JUNE_5_2016)
#define ELEMENTS_GALLERY_ICON_BUTTON_JUNE_5_2016

#include <elements/element/element.hpp>
#include <elements/element/gallery/button.hpp>
#include <elements/support/theme.hpp>

namespace cycfi::elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Icon Buttons
   ////////////////////////////////////////////////////////////////////////////
   struct icon_button_styler_base : element, basic_receiver<button_state>
   {
                              icon_button_styler_base(float size)
                               : _size(size)
                              {}

      view_limits             limits(basic_context const& ctx) const override;

      float                   _size;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Icon button with body
   ////////////////////////////////////////////////////////////////////////////
   struct icon_button_styler : icon_button_styler_base
   {
                              icon_button_styler(
                                 uint32_t code
                               , float size
                               , color body_color = get_theme().default_button_color
                              )
                               : icon_button_styler_base(size)
                               , _code(code)
                               , _body_color(body_color)
                              {}

      void                    draw(context const& ctx) override;

      uint32_t                _code;
      color                   _body_color;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Icon button with body and two alternating icons
   ////////////////////////////////////////////////////////////////////////////
   struct icon_button_styler2 : icon_button_styler_base
   {
                              icon_button_styler2(
                                 uint32_t code1
                               , uint32_t code2
                               , float size
                               , color body_color = get_theme().default_button_color
                              )
                               : icon_button_styler_base(size)
                               , _code1(code1)
                               , _code2(code2)
                               , _body_color(body_color)
                              {}

      void                    draw(context const& ctx) override;

      uint32_t                _code1;
      uint32_t                _code2;
      color                   _body_color;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Icon button without body
   ////////////////////////////////////////////////////////////////////////////
   struct
   [[deprecated("If you want a plain icon button, just make a standard icon_button with a fully transparent color (e.g. rgba(0, 0, 0, 0)")]]
   plain_icon_button_styler : icon_button_styler_base
   {
                              plain_icon_button_styler(
                                 uint32_t code
                               , float size
                              )
                               : icon_button_styler_base(size)
                               , _code(code)
                              {}

      void                    draw(context const& ctx) override;

      uint32_t                _code;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Make toggle icon_button
   ////////////////////////////////////////////////////////////////////////////
   inline auto toggle_icon_button(
      uint32_t code
    , float size
    , color body_color = get_theme().default_button_color
   )
   {
      return toggle_button(icon_button_styler{code, size, body_color});
   }

   ////////////////////////////////////////////////////////////////////////////
   // Make toggle icon_button with two alternating icons
   ////////////////////////////////////////////////////////////////////////////
   inline auto toggle_icon_button(
      uint32_t code1
    , uint32_t code2
    , float size
    , color body_color = get_theme().default_button_color
   )
   {
      return toggle_button(icon_button_styler2{code1, code2, size, body_color});
   }

   ////////////////////////////////////////////////////////////////////////////
   // Make momentary icon_button
   ////////////////////////////////////////////////////////////////////////////
   inline auto icon_button(
      uint32_t code
    , float size = 1.0f
    , color body_color = get_theme().default_button_color
   )
   {
      return momentary_button(icon_button_styler{code, size, body_color});
   }

   ////////////////////////////////////////////////////////////////////////////
   // Make momentary icon_button without a body
   ////////////////////////////////////////////////////////////////////////////
   inline auto plain_icon_button(uint32_t code, float size = 1.0f)
   {
      return momentary_button(icon_button_styler{code, size, rgba(0, 0, 0, 0)});
   }
}

#endif
