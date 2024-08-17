/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_STYLE_SLIDE_SWITCH_NOVEMBER_10_2023)
#define ELEMENTS_STYLE_SLIDE_SWITCH_NOVEMBER_10_2023

#include <elements/element/style/button.hpp>

namespace cycfi::elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Slide Switch
   ////////////////////////////////////////////////////////////////////////////
   class slide_switch_styler : public element
   {
   public:

      view_limits       limits(basic_context const& ctx) const override;
      void              draw(context const& ctx) override;
      bool              wants_control() const override;

   private:

      float             _val = 0.0f;
   };

   inline auto slide_switch()
   {
      return toggle_button(slide_switch_styler{});
   }
}

#endif
