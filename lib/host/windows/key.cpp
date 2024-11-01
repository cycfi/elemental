/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)

   Key mapping ported to C++ from GLFW3

   Copyright (c) 2009-2016 Camilla Berglund <elmindreda@glfw.org>

   This software is provided 'as-is', without any express or implied
   warranty. In no event will the authors be held liable for any damages
   arising from the use of this software.

   Permission is granted to anyone to use this software for any purpose,
   including commercial applications, and to alter it and redistribute it
   freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not
      claim that you wrote the original software. If you use this software
      in a product, an acknowledgment in the product documentation would
      be appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not
      be misrepresented as being the original software.

   3. This notice may not be removed or altered from any source
      distribution.

=============================================================================*/
#include <elements/base_view.hpp>

namespace cycfi::elements
{
   key_code translate_key(WPARAM wparam, LPARAM lparam)
   {
      // The Ctrl keys require special handling
      if (wparam == VK_CONTROL)
      {
         MSG next;
         DWORD time;

        // Right side keys have the extended key bit set
         if (lparam & 0x01000000)
            return key_code::right_control;

         // HACK: Alt Gr sends Left Ctrl and then Right Alt in close sequence
         //       We only want the Right Alt message, so if the next message is
         //       Right Alt we ignore this (synthetic) Left Ctrl message
         time = GetMessageTime();

         if (PeekMessageW(&next, nullptr, 0, 0, PM_NOREMOVE))
         {
            if (next.message == WM_KEYDOWN ||
                next.message == WM_SYSKEYDOWN ||
                next.message == WM_KEYUP ||
                next.message == WM_SYSKEYUP)
            {
               if (next.wParam == VK_MENU &&
                  (next.lParam & 0x01000000) &&
                  next.time == time)
               {
                  // Next message is Right Alt down so discard this
                  return key_code::unknown;
               }
            }
         }

         return key_code::left_control;
      }

      if (wparam == VK_PROCESSKEY)
      {
         // IME notifies that keys have been filtered by setting the virtual
         // key-code to VK_PROCESSKEY
         return key_code::ime_process_key;
      }

      unsigned int key = HIWORD(lparam) & 0x1FF;
      switch (key)
      {
         default:    return key_code::unknown;

         case 0x00b:  return key_code:: _0;
         case 0x002:  return key_code:: _1;
         case 0x003:  return key_code:: _2;
         case 0x004:  return key_code:: _3;
         case 0x005:  return key_code:: _4;
         case 0x006:  return key_code:: _5;
         case 0x007:  return key_code:: _6;
         case 0x008:  return key_code:: _7;
         case 0x009:  return key_code:: _8;
         case 0x00a:  return key_code:: _9;

         case 0x01e:  return key_code::a;
         case 0x030:  return key_code::b;
         case 0x02e:  return key_code::c;
         case 0x020:  return key_code::d;
         case 0x012:  return key_code::e;
         case 0x021:  return key_code::f;
         case 0x022:  return key_code::g;
         case 0x023:  return key_code::h;
         case 0x017:  return key_code::i;
         case 0x024:  return key_code::j;
         case 0x025:  return key_code::k;
         case 0x026:  return key_code::l;
         case 0x032:  return key_code::m;
         case 0x031:  return key_code::n;
         case 0x018:  return key_code::o;
         case 0x019:  return key_code::p;
         case 0x010:  return key_code::q;
         case 0x013:  return key_code::r;
         case 0x01f:  return key_code::s;
         case 0x014:  return key_code::t;
         case 0x016:  return key_code::u;
         case 0x02f:  return key_code::v;
         case 0x011:  return key_code::w;
         case 0x02d:  return key_code::x;
         case 0x015:  return key_code::y;
         case 0x02c:  return key_code::z;

         case 0x028:  return key_code::apostrophe;
         case 0x02b:  return key_code::backslash;
         case 0x033:  return key_code::comma;
         case 0x00d:  return key_code::equal;
         case 0x029:  return key_code::grave_accent;
         case 0x01a:  return key_code::left_bracket;
         case 0x00c:  return key_code::minus;
         case 0x034:  return key_code::period;
         case 0x01b:  return key_code::right_bracket;
         case 0x027:  return key_code::semicolon;
         case 0x035:  return key_code::slash;
         case 0x056:  return key_code::world_2;

         case 0x00e:  return key_code::backspace;
         case 0x153:  return key_code::_delete;
         case 0x14f:  return key_code::end;
         case 0x01c:  return key_code::enter;
         case 0x001:  return key_code::escape;
         case 0x147:  return key_code::home;
         case 0x152:  return key_code::insert;
         case 0x15d:  return key_code::menu;
         case 0x151:  return key_code::page_down;
         case 0x149:  return key_code::page_up;
         case 0x045:  return key_code::pause;
         case 0x146:  return key_code::pause;
         case 0x039:  return key_code::space;
         case 0x00f:  return key_code::tab;
         case 0x03a:  return key_code::caps_lock;
         case 0x145:  return key_code::num_lock;
         case 0x046:  return key_code::scroll_lock;

         case 0x03b:  return key_code::f1;
         case 0x03c:  return key_code::f2;
         case 0x03d:  return key_code::f3;
         case 0x03e:  return key_code::f4;
         case 0x03f:  return key_code::f5;
         case 0x040:  return key_code::f6;
         case 0x041:  return key_code::f7;
         case 0x042:  return key_code::f8;
         case 0x043:  return key_code::f9;
         case 0x044:  return key_code::f10;
         case 0x057:  return key_code::f11;
         case 0x058:  return key_code::f12;
         case 0x064:  return key_code::f13;
         case 0x065:  return key_code::f14;
         case 0x066:  return key_code::f15;
         case 0x067:  return key_code::f16;
         case 0x068:  return key_code::f17;
         case 0x069:  return key_code::f18;
         case 0x06a:  return key_code::f19;
         case 0x06b:  return key_code::f20;
         case 0x06c:  return key_code::f21;
         case 0x06d:  return key_code::f22;
         case 0x06e:  return key_code::f23;
         case 0x076:  return key_code::f24;

         case 0x038:  return key_code::left_alt;
         case 0x01d:  return key_code::left_control;
         case 0x02a:  return key_code::left_shift;
         case 0x15b:  return key_code::left_super;
         case 0x137:  return key_code::print_screen;
         case 0x138:  return key_code::right_alt;
         case 0x11d:  return key_code::right_control;
         case 0x036:  return key_code::right_shift;
         case 0x15c:  return key_code::right_super;
         case 0x150:  return key_code::down;
         case 0x14b:  return key_code::left;
         case 0x14d:  return key_code::right;
         case 0x148:  return key_code::up;

         case 0x052:  return key_code::kp_0;
         case 0x04f:  return key_code::kp_1;
         case 0x050:  return key_code::kp_2;
         case 0x051:  return key_code::kp_3;
         case 0x04b:  return key_code::kp_4;
         case 0x04c:  return key_code::kp_5;
         case 0x04d:  return key_code::kp_6;
         case 0x047:  return key_code::kp_7;
         case 0x048:  return key_code::kp_8;
         case 0x049:  return key_code::kp_9;
         case 0x04e:  return key_code::kp_add;
         case 0x053:  return key_code::kp_decimal;
         case 0x135:  return key_code::kp_divide;
         case 0x11c:  return key_code::kp_enter;
         case 0x059:  return key_code::kp_equal;
         case 0x037:  return key_code::kp_multiply;
         case 0x04a:  return key_code::kp_subtract;
      }
      return key_code::unknown;
   }
}
