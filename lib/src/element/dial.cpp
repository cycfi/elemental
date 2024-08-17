/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/dial.hpp>
#include <elements/element/traversal.hpp>
#include <elements/support/theme.hpp>
#include <elements/view.hpp>
#include <cmath>

namespace cycfi::elements
{
   ////////////////////////////////////////////////////////////////////////////
   // basic_dial
   ////////////////////////////////////////////////////////////////////////////
   basic_dial::basic_dial(double init_value)
    : _value(init_value)
   {
      clamp(_value, 0.0, 1.0);
   }

   void basic_dial::value(double val)
   {
      _value = clamp(val, 0.0, 1.0);
   }

   namespace
   {
      inline void edit_value(basic_dial* this_, double val)
      {
         this_->value(val);
         if (this_->on_change)
            this_->on_change(this_->value());
      }
   }

   void basic_dial::edit(view& /*view_*/, param_type val)
   {
      edit_value(this, val);
   }

   double basic_dial::radial_value(context const& ctx, tracker_info& track_info)
   {
      using namespace radial_consts;

      point p = track_info.current;
      point center = center_point(ctx.bounds);
      double angle = -std::atan2(p.x-center.x, p.y-center.y);
      if (angle < 0.0)
         angle += _2pi;

      float val = (angle-start_angle) / range;
      if (std::abs(val - value()) < 0.6)
         return clamp(val, 0.0, 1.0);
      return value();
   }

   double basic_dial::linear_value(context const& /*ctx*/, tracker_info& track_info)
   {
      point delta{
         track_info.current.x - track_info.previous.x,
         track_info.current.y - track_info.previous.y
      };

      double factor = 1.0 / get_theme().dial_linear_range;
      if (track_info.modifiers & mod_shift)
         factor /= 5.0;

      float val = _value + factor * (delta.x - delta.y);
      return clamp(val, 0.0, 1.0);
   }

   double basic_dial::compute_value(context const& ctx, tracker_info& track_info)
   {
      return (get_theme().dial_mode == dial_mode_enum::radial)?
         radial_value(ctx, track_info) :
         linear_value(ctx, track_info)
         ;
   }

   void basic_dial::keep_tracking(context const& ctx, tracker_info& track_info)
   {
      if (track_info.current != track_info.previous)
      {
         double new_value = compute_value(ctx, track_info);
         if (_value != new_value)
         {
            edit_value(this, new_value);
            ctx.view.refresh(ctx);
         }
      }
   }

   bool basic_dial::scroll(context const& ctx, point dir, point p)
   {
      auto sdir = scroll_direction();
      track_scroll(ctx, dir, p);
      edit_value(this, value()
         + (-sdir.y * dir.y * 0.005)
         + (sdir.x * dir.x * 0.005)
      );
      ctx.view.refresh(ctx);
      return true;
   }
}
