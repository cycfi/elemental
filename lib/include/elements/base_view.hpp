/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_ELEMENTS_BASE_VIEW_AUGUST_20_2016)
#define CYCFI_ELEMENTS_BASE_VIEW_AUGUST_20_2016

#include <utility>
#include <memory>
#include <string>
#include <vector>
#include <cstdint>
#include <functional>
#include <cairo.h>

#include <infra/support.hpp>
#include <infra/filesystem.hpp>
#include <elements/support/point.hpp>
#include <elements/support/rect.hpp>
#include <elements/support/payload.hpp>

#if defined(ELEMENTS_HOST_UI_LIBRARY_WIN32)
# include <windows.h>
#endif

namespace cycfi::elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Mouse Button
   ////////////////////////////////////////////////////////////////////////////
   struct mouse_button
   {
      enum what { left, middle, right };

      bool     down;
      int      num_clicks;
      what     state;
      int      modifiers;
      point    pos;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Cursor tracking
   ////////////////////////////////////////////////////////////////////////////
   enum class cursor_tracking
   {
      entering,   // Sent when the cursor is entering
      hovering,   // Sent when the cursor is hovering
      leaving     // Sent when the cursor is leaving
   };

   ////////////////////////////////////////////////////////////////////////////
   // View Limits
   ////////////////////////////////////////////////////////////////////////////
   constexpr float full_extent    = 1E30;

   struct view_limits
   {
      point    min = {0.0, 0.0};
      point    max = {full_extent, full_extent};
   };

   constexpr view_limits full_limits = {
      {0.0, 0.0}
    , {full_extent, full_extent}
   };

   ////////////////////////////////////////////////////////////////////////////
   // View stretch
   ////////////////////////////////////////////////////////////////////////////
   struct view_stretch
   {
      float    x = 1.0;
      float    y = 1.0;

      constexpr float&  operator[](axis a);
      constexpr float   operator[](axis a) const;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Text info
   ////////////////////////////////////////////////////////////////////////////
   struct text_info
   {
      uint32_t codepoint;
      int      modifiers;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Keyboard information
   //
   // Key mapping ported to C++ from GLFW3
   //
   // Copyright (c) 2009-2016 Camilla Berglund <elmindreda@glfw.org>
   //
   // This software is provided 'as-is', without any express or implied
   // warranty. In no event will the authors be held liable for any damages
   // arising from the use of this software.
   //
   // Permission is granted to anyone to use this software for any purpose,
   // including commercial applications, and to alter it and redistribute it
   // freely, subject to the following restrictions:
   //
   // 1. The origin of this software must not be misrepresented; you must not
   //    claim that you wrote the original software. If you use this software
   //    in a product, an acknowledgment in the product documentation would
   //    be appreciated but is not required.
   //
   // 2. Altered source versions must be plainly marked as such, and must not
   //    be misrepresented as being the original software.
   //
   // 3. This notice may not be removed or altered from any source
   //    distribution.
   //
   ////////////////////////////////////////////////////////////////////////////
   enum class key_action
   {
      unknown           = -1,
      release           = 0,
      press             = 1,
      repeat            = 2,
   };

   enum : uint16_t
   {
      mod_shift         = 0x0001,
      mod_control       = 0x0002,

      // mod_alt (alias mod_option) maps to the Alt key on PC keyboards
      // and maps to the Option key on MacOS
      mod_alt           = 0x0004,
      mod_option        = mod_alt,     // synonym for mod_alt

      // mod_super maps to the Windows key on PC keyboards
      // and maps to the Command key on MacOS
      mod_super         = 0x0008,
      mod_command       = mod_super,   // synonym for MacOS

      // mod_action maps to mod_control on Windows and Linux
      // and maps to mod_super on MacOS
      mod_action        = 0x0010,
   };

   enum class key_code : int16_t
   {
#ifdef WIN32
      ime_process_key   = -2,
#endif
      unknown           = -1,

      // Printable keys
      space             = 32,
      apostrophe        = 39,    // '
      comma             = 44,    // ,
      minus             = 45,    // -
      period            = 46,    // .
      slash             = 47,    // /
      _0                = 48,
      _1                = 49,
      _2                = 50,
      _3                = 51,
      _4                = 52,
      _5                = 53,
      _6                = 54,
      _7                = 55,
      _8                = 56,
      _9                = 57,
      semicolon         = 59,    // ;
      equal             = 61,    // =
      a                 = 65,
      b                 = 66,
      c                 = 67,
      d                 = 68,
      e                 = 69,
      f                 = 70,
      g                 = 71,
      h                 = 72,
      i                 = 73,
      j                 = 74,
      k                 = 75,
      l                 = 76,
      m                 = 77,
      n                 = 78,
      o                 = 79,
      p                 = 80,
      q                 = 81,
      r                 = 82,
      s                 = 83,
      t                 = 84,
      u                 = 85,
      v                 = 86,
      w                 = 87,
      x                 = 88,
      y                 = 89,
      z                 = 90,
      left_bracket      = 91,    // [
      backslash         = 92,    // \ (back-slash)
      right_bracket     = 93,    // ]
      grave_accent      = 96,    // `
      world_1           = 161,   // non-US #1
      world_2           = 162,   // non-US #2

      // Function keys
      escape            = 256,
      enter             = 257,
      tab               = 258,
      backspace         = 259,
      insert            = 260,
      _delete           = 261,
      right             = 262,
      left              = 263,
      down              = 264,
      up                = 265,
      page_up           = 266,
      page_down         = 267,
      home              = 268,
      end               = 269,
      caps_lock         = 280,
      scroll_lock       = 281,
      num_lock          = 282,
      print_screen      = 283,
      pause             = 284,
      f1                = 290,
      f2                = 291,
      f3                = 292,
      f4                = 293,
      f5                = 294,
      f6                = 295,
      f7                = 296,
      f8                = 297,
      f9                = 298,
      f10               = 299,
      f11               = 300,
      f12               = 301,
      f13               = 302,
      f14               = 303,
      f15               = 304,
      f16               = 305,
      f17               = 306,
      f18               = 307,
      f19               = 308,
      f20               = 309,
      f21               = 310,
      f22               = 311,
      f23               = 312,
      f24               = 313,
      f25               = 314,
      kp_0              = 320,
      kp_1              = 321,
      kp_2              = 322,
      kp_3              = 323,
      kp_4              = 324,
      kp_5              = 325,
      kp_6              = 326,
      kp_7              = 327,
      kp_8              = 328,
      kp_9              = 329,
      kp_decimal        = 330,
      kp_divide         = 331,
      kp_multiply       = 332,
      kp_subtract       = 333,
      kp_add            = 334,
      kp_enter          = 335,
      kp_equal          = 336,
      left_shift        = 340,
      left_control      = 341,
      left_alt          = 342,
      left_super        = 343,
      right_shift       = 344,
      right_control     = 345,
      right_alt         = 346,
      right_super       = 347,
      menu              = 348,

      last              = menu,
   };

   struct key_info
   {
      key_code          key;
      key_action        action;
      int               modifiers;
   };

   /**
    * \struct drop_info
    *
    * \brief
    *    A structure encapsulating information about a drag-and-drop
    *    operation.
    *
    *    The `drop_info` structure contains information about a drop
    *    operation, including the payload data and the drop location.
    *
    * \var payload data
    *    The payload that is being transferred during the drag-and-drop
    *    operation (see payload for more info).
    *
    * \var mutable point where
    *    The location where the drop operation takes place.
    */
   struct drop_info
   {
      payload           data;
      mutable point     where;
   };

   ////////////////////////////////////////////////////////////////////////////
   // The base view base class
   ////////////////////////////////////////////////////////////////////////////

#if defined(ELEMENTS_HOST_UI_LIBRARY_WIN32)
   using host_view_handle = HWND;
   using host_window_handle = HWND;
#else
   struct host_view;
   using host_view_handle = host_view*;
   struct host_window;
   using host_window_handle = host_window*;
#endif

   class base_view : non_copyable
   {
   public:

#if defined(ELEMENTS_HOST_UI_LIBRARY_COCOA) || defined(ELEMENTS_HOST_UI_LIBRARY_GTK)
                           base_view(host_view_handle h);
#endif
                           base_view(extent size_);
                           base_view(host_window_handle h);
      virtual              ~base_view();

      virtual void         draw(cairo_t* ctx);
      virtual void         click(mouse_button btn);
      virtual void         drag(mouse_button btn);
      virtual void         cursor(point p, cursor_tracking status);
      virtual void         scroll(point dir, point p);
      virtual bool         key(key_info const& k);
      virtual bool         text(text_info const& info);
      virtual void         begin_focus();
      virtual void         end_focus();
      virtual void         track_drop(drop_info const& info, cursor_tracking status);
      virtual bool         drop(drop_info const& info);
      virtual void         poll();

      virtual void         refresh();
      virtual void         refresh(rect area);

      point                cursor_pos() const;
      extent               size() const;
      void                 size(extent size_);
      host_view_handle     host() const { return _view; }

   private:

      host_view_handle     _view;
   };

   ////////////////////////////////////////////////////////////////////////////
   inline void base_view::draw(cairo_t* /* ctx */) {}
   inline void base_view::click(mouse_button /* btn */) {}
   inline void base_view::drag(mouse_button /* btn */) {}
   inline void base_view::cursor(point /* p */, cursor_tracking /* status */) {}
   inline void base_view::scroll(point /* dir */, point /* p */) {}
   inline bool base_view::key(key_info const& /* k */) { return false; }
   inline bool base_view::text(text_info const& /* info */) { return false; }
   inline void base_view::begin_focus() {}
   inline void base_view::end_focus() {}
   inline void base_view::track_drop(drop_info const& /*info*/, cursor_tracking /* status */) {}
   inline bool base_view::drop(drop_info const& /*info*/)
   {
      return false;
   }
   inline void base_view::poll() {}

   ////////////////////////////////////////////////////////////////////////////
   // The clipboard
   std::string clipboard();
   void clipboard(std::string const& text);

   ////////////////////////////////////////////////////////////////////////////
   // The Cursor
   enum class cursor_type
   {
      arrow,
      ibeam,
      cross_hair,
      hand,
      h_resize,
      v_resize
   };

   void set_cursor(cursor_type type);

   ////////////////////////////////////////////////////////////////////////////
   // Scroll direction
   point scroll_direction();

   ////////////////////////////////////////////////////////////////////////////
   constexpr float& view_stretch::operator[](axis a)
   {
      return a==axis::x ? x : y;
   }

   constexpr float view_stretch::operator[](axis a) const
   {
     return a==axis::x ? x : y;
   }
}

#endif
