/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_STYLE_CHILD_WINDOW_JANUARY_19_2021)
#define ELEMENTS_STYLE_CHILD_WINDOW_JANUARY_19_2021

#include <elements/element/child_window.hpp>
#include <elements/element/style/pane.hpp>
#include <elements/element/style/icon_button.hpp>

namespace cycfi::elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Basic Child Window (movable, non-closable)
   ////////////////////////////////////////////////////////////////////////////
   template <typename Content>
   auto basic_child_window(std::string title, rect bounds, Content&& content)
   {
      return child_window(bounds,
         pane_ex(
            movable(title_bar{}),
            title,
            std::forward<Content>(content),
            get_theme().child_window_title_size,
            get_theme().child_window_opacity
         )
      );
   }

   template <typename Content>
   auto basic_child_window(std::string title, point pos, Content&& content)
   {
      return basic_child_window(
         title,
         rect{pos.x, pos.y, pos.x, pos.y},
         std::forward<Content>(content)
      );
   }

   ////////////////////////////////////////////////////////////////////////////
   // Closable Child Window (movable and closable)
   ////////////////////////////////////////////////////////////////////////////
   template <typename Content>
   [[deprecated("Use standard_child_window(...) instead")]]
   auto closable_child_window(std::string title, rect bounds, Content&& content)
   {
      return child_window(bounds,
         pane_ex(
            movable(
               layer(
                  closable(
                     align_right_middle(
                        plain_icon_button(icons::cancel, 0.8)
                     )
                  ),
                  title_bar{}
               )
            ),
            title,
            std::forward<Content>(content),
            get_theme().child_window_title_size,
            get_theme().child_window_opacity
         )
      );
   }

   template <typename Content>
   [[deprecated("Use standard_child_window(...) instead")]]
   auto closable_child_window(std::string title, point pos, Content&& content)
   {
      return closable_child_window(
         title,
         rect{pos.x, pos.y, pos.x, pos.y},
         std::forward<Content>(content)
      );
   }

   template <typename Content>
   auto standard_child_window(std::string title, rect bounds, Content&& content)
   {
      return child_window(bounds,
         resizable(
            pane_ex(
               movable(
                  layer(
                     closable(
                        align_right_middle(
                           plain_icon_button(icons::cancel, 0.8)
                        )
                     ),
                     title_bar{}
                  )
               ),
               title,
               std::forward<Content>(content),
               get_theme().child_window_title_size,
               get_theme().child_window_opacity
            )
         )
      );
   }

   template <typename Content>
   auto standard_child_window(std::string title, point pos, Content&& content)
   {
      return standard_child_window(
         title,
         rect{pos.x, pos.y, pos.x, pos.y},
         std::forward<Content>(content)
      );
   }
}

#endif
