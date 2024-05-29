/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_MISC_APRIL_11_2016)
#define ELEMENTS_MISC_APRIL_11_2016

#include <elements/element/element.hpp>
#include <elements/element/proxy.hpp>
#include <elements/element/text.hpp>
#include <elements/element/size.hpp>
#include <elements/support/theme.hpp>
#include <infra/support.hpp>

#include <functional>
#include <utility>

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // spacers: empty horizontal or vertical element with specified width or
   // height.
   ////////////////////////////////////////////////////////////////////////////
   [[deprecated("Use vspace(size) instead.")]]
   inline auto vspacer(float size)
   {
      return vsize(size, element{});
   }

   [[deprecated("Use hspace(size) instead.")]]
   inline auto hspacer(float size)
   {
      return hsize(size, element{});
   }

   ////////////////////////////////////////////////////////////////////////////
   // Box: A simple colored box.
   ////////////////////////////////////////////////////////////////////////////
   struct box_element : element
   {
      box_element(color color_)
       : _color(color_)
      {}

      void draw(context const& ctx) override
      {
         auto& cnv = ctx.canvas;
         cnv.fill_style(_color);
         cnv.fill_rect(ctx.bounds);
      }

      color _color;
   };

   inline auto box(color color_)
   {
      return box_element{color_};
   }

   ////////////////////////////////////////////////////////////////////////////
   // RBox: A simple colored rounded-box.
   ////////////////////////////////////////////////////////////////////////////
   struct rbox_element : element
   {
      rbox_element(color color_, float radius = 4)
       : _color(color_)
       , _radius(radius)
      {}

      void draw(context const& ctx) override
      {
         auto& cnv = ctx.canvas;
         cnv.begin_path();
         cnv.add_round_rect(ctx.bounds, _radius);
         cnv.fill_style(_color);
         cnv.fill();
      }

      color _color;
      float _radius;
   };

   inline auto rbox(color color_, float radius = 4)
   {
      return rbox_element{color_, radius};
   }

   ////////////////////////////////////////////////////////////////////////////
   // Draw Element
   //
   // The draw element takes in a function that draws something
   ////////////////////////////////////////////////////////////////////////////
   template <typename F>
   class draw_element : public element
   {
   public:

      draw_element(F draw)
       : _draw(draw)
      {}

      void draw(context const& ctx) override
      {
         _draw(ctx);
      }

   private:

      F _draw;
   };

   template <typename F>
   [[deprecated("Use draw(F&& _draw) instead.")]]
   inline draw_element<F> basic(F&& _draw)
   {
      return {std::forward<remove_cvref_t<F>>(_draw)};
   }

   template <typename F>
   inline draw_element<F> draw(F&& _draw)
   {
      using ftype = remove_cvref_t<F>;
      return {std::forward<ftype>(_draw)};
   }

   ////////////////////////////////////////////////////////////////////////////
   // Draw Value Element
   //
   // The draw_value_element takes in a function that draws something based
   // on the received value (see support/receiver.hpp)
   ////////////////////////////////////////////////////////////////////////////
   template <typename T, typename F>
   class draw_value_element : public basic_receiver<T>, public element
   {
   public:

      draw_value_element(F draw)
       : _draw(draw)
      {}

      void draw(context const& ctx) override
      {
         _draw(ctx, this->value());
      }

   private:

      F _draw;
      T _value;
   };

   template <typename T, typename F>
   inline draw_value_element<T, remove_cvref_t<F>> draw_value(F&& f)
   {
      using ftype = remove_cvref_t<F>;
      return {std::forward<ftype>(f)};
   }

   ////////////////////////////////////////////////////////////////////////////
   // Panels
   ////////////////////////////////////////////////////////////////////////////
   class panel : public element
   {
   public:
                     panel(float opacity_ = get_theme().panel_color.alpha)
                      : _opacity(opacity_)
                     {}

      void           draw(context const& ctx) override;

   private:

      float          _opacity;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Frames and borders
   ////////////////////////////////////////////////////////////////////////////
   struct frame : public element
   {
      void           draw(context const& ctx) override;
   };

   struct border : public element
   {
      void           draw(context const& ctx) override;
   };

   struct border_left : public element
   {
      void           draw(context const& ctx) override;
   };

   struct border_right : public element
   {
      void           draw(context const& ctx) override;
   };

   struct border_top : public element
   {
      void           draw(context const& ctx) override;
   };

   struct border_bottom : public element
   {
      void           draw(context const& ctx) override;
   };

   struct border_top_bottom : public element
   {
      void           draw(context const& ctx) override;
   };

   struct border_left_right : public element
   {
      void           draw(context const& ctx) override;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Title Bars
   ////////////////////////////////////////////////////////////////////////////
   class title_bar : public element
   {
   public:

      void           draw(context const& ctx) override;
   };

   inline void title_bar::draw(context const& ctx)
   {
      draw_box_vgradient(ctx.canvas, ctx.bounds, 4.0);
   }

   ////////////////////////////////////////////////////////////////////////////
   // Grid Lines - Vertical
   ////////////////////////////////////////////////////////////////////////////
   class vgrid_lines : public element
   {
   public:

                     vgrid_lines(float major_divisions, float minor_divisions)
                      : _major_divisions(major_divisions)
                      , _minor_divisions(minor_divisions)
                     {}

      void           draw(context const& ctx) override;

   private:

      float          _major_divisions;
      float          _minor_divisions;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Grid Lines - Horizontal
   ////////////////////////////////////////////////////////////////////////////
   class hgrid_lines : public element
   {
   public:

                     hgrid_lines(float major_divisions, float minor_divisions)
                      : _major_divisions(major_divisions)
                      , _minor_divisions(minor_divisions)
                     {}

      void           draw(context const& ctx) override;

   private:

      float          _major_divisions;
      float          _minor_divisions;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Icons
   ////////////////////////////////////////////////////////////////////////////
   struct icon : element
   {
                     icon(std::uint32_t code_, float size_ = 1.0);

      view_limits    limits(basic_context const& ctx) const override;
      void           draw(context const& ctx) override;

      std::uint32_t  _code;
      float          _size;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Key Intercept
   ////////////////////////////////////////////////////////////////////////////
   template <typename Subject>
   struct key_intercept_element : public proxy<Subject>
   {
      using base_type = proxy<Subject>;

                     key_intercept_element(Subject subject)
                      : base_type(std::move(subject))
                     {}

      bool           key(context const& ctx, key_info k) override;
      bool           wants_control() const override { return true; }
      bool           wants_focus() const override { return true; }

      using key_function = std::function<bool(key_info k)>;

      key_function   on_key = [](auto){ return false; };
   };

   template <typename Subject>
   inline key_intercept_element<remove_cvref_t<Subject>>
   key_intercept(Subject&& subject)
   {
      return {std::forward<Subject>(subject)};
   }

   template <typename Subject>
   inline bool key_intercept_element<Subject>::key(context const& ctx, key_info k)
   {
      if (on_key(k))
         return true;
      return this->subject().key(ctx, k);
   }

   ////////////////////////////////////////////////////////////////////////////
   // Text Intercept
   ////////////////////////////////////////////////////////////////////////////
   template <typename Subject>
   struct text_intercept_element : public proxy<Subject>
   {
      using base_type = proxy<Subject>;

                     text_intercept_element(Subject subject)
                      : base_type(std::move(subject))
                     {}

      bool           text(context const& ctx, text_info info) override;
      bool           wants_control() const override { return true; }
      bool           wants_focus() const override { return true; }

      using text_function = std::function<bool(text_info info)>;

      text_function   on_text = [](auto){ return false; };
   };

   template <typename Subject>
   inline text_intercept_element<remove_cvref_t<Subject>>
   text_intercept(Subject&& subject)
   {
      return {std::forward<Subject>(subject)};
   }

   template <typename Subject>
   inline bool text_intercept_element<Subject>::text(context const& ctx, text_info info)
   {
      if (on_text(info))
         return true;
      return this->subject().text(ctx, info);
   }

   ////////////////////////////////////////////////////////////////////////////
   // Click Intercept
   ////////////////////////////////////////////////////////////////////////////
   template <typename Subject>
   struct click_intercept_element : public proxy<Subject>
   {
      using base_type = proxy<Subject>;

                     click_intercept_element(Subject subject)
                      : base_type(std::move(subject))
                     {}

      bool           click(context const& ctx, mouse_button btn) override;
      bool           wants_control() const override { return true; }
      bool           wants_focus() const override { return true; }

      using click_function = std::function<bool(mouse_button btn)>;

      click_function on_click = [](auto){ return false; };
   };

   template <typename Subject>
   inline click_intercept_element<remove_cvref_t<Subject>>
   click_intercept(Subject&& subject)
   {
      return {std::forward<Subject>(subject)};
   }

   template <typename Subject>
   inline bool click_intercept_element<Subject>::click(context const& ctx, mouse_button btn)
   {
      if (on_click(btn))
         return true;
      return this->subject().click(ctx, btn);
   }

   ////////////////////////////////////////////////////////////////////////////
   // Hidable
   ////////////////////////////////////////////////////////////////////////////
   template <typename Subject>
   class hidable_element : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;

                              hidable_element(Subject subject);
      void                    draw(context const& ctx) override;
      bool                    wants_control() const override;
      bool                    wants_focus() const override;
      bool                    is_hidden = false;
   };

   template <typename Subject>
   inline hidable_element<Subject>::hidable_element(Subject subject)
    : base_type(std::move(subject))
   {}

   template <typename Subject>
   inline void hidable_element<Subject>::draw(context const& ctx)
   {
      if (!is_hidden)
         this->subject().draw(ctx);
   }

   template <typename Subject>
   inline bool hidable_element<Subject>::wants_control() const
   {
      if (is_hidden)
         return false;
      return this->subject().wants_control();
   }

   template <typename Subject>
   inline bool hidable_element<Subject>::wants_focus() const
   {
      if (is_hidden)
         return false;
      return this->subject().wants_focus();
   }

   template <typename Subject>
   inline hidable_element<remove_cvref_t<Subject>>
   hidable(Subject&& subject)
   {
      return {std::forward<Subject>(subject)};
   }

   ////////////////////////////////////////////////////////////////////////////
   // Vertical collapsable
   ////////////////////////////////////////////////////////////////////////////
   template <typename Subject>
   class vcollapsable_element : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;

                              vcollapsable_element(Subject subject);
      view_limits             limits(basic_context const& ctx) const override;
      void                    draw(context const& ctx) override;
      bool                    wants_control() const override;
      bool                    wants_focus() const override;
      bool                    is_collapsed = false;
   };

   template <typename Subject>
   inline vcollapsable_element<Subject>::vcollapsable_element(Subject subject)
    : base_type(std::move(subject))
   {}

   template <typename Subject>
   inline view_limits
   vcollapsable_element<Subject>::limits(basic_context const& ctx) const
   {
      auto e_limits = this->subject().limits(ctx);
      return is_collapsed?
         view_limits{{e_limits.min.x, 0.0f}, {e_limits.max.x, 0.0f}} :
         e_limits;
   }

   template <typename Subject>
   inline void vcollapsable_element<Subject>::draw(context const& ctx)
   {
      if (!is_collapsed)
         this->subject().draw(ctx);
   }

   template <typename Subject>
   inline bool vcollapsable_element<Subject>::wants_control() const
   {
      if (is_collapsed)
         return false;
      return this->subject().wants_control();
   }

   template <typename Subject>
   inline bool vcollapsable_element<Subject>::wants_focus() const
   {
      if (is_collapsed)
         return false;
      return this->subject().wants_focus();
   }

   template <typename Subject>
   inline vcollapsable_element<remove_cvref_t<Subject>>
   vcollapsable(Subject&& subject)
   {
      return {std::forward<Subject>(subject)};
   }

   ////////////////////////////////////////////////////////////////////////////
   // Modal element hugs the UI and prevents any event from passing through
   ////////////////////////////////////////////////////////////////////////////
   template <typename Subject>
   class modal_element : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;

                     modal_element(Subject subject);
      element*       hit_test(context const& ctx, point p, bool leaf, bool control) override;
      bool           click(context const& ctx, mouse_button btn) override;
      bool           key(context const& ctx, key_info k) override;
      bool           text(context const& ctx, text_info info) override;

      bool           wants_focus() const override { return true; }
      bool           wants_control() const override { return true; }
   };

   template <typename Subject>
   inline modal_element<remove_cvref_t<Subject>>
   modal(Subject&& subject)
   {
      return {std::forward<Subject>(subject)};
   }

   template <typename Subject>
   inline modal_element<Subject>::modal_element(Subject subject)
    : base_type(std::move(subject))
   {
   }

   template <typename Subject>
   inline element* modal_element<Subject>::hit_test(context const& ctx, point p, bool leaf, bool control)
   {
      if (auto e = this->subject().hit_test(ctx, p, leaf, control))
         return e;
      return this;
   }

   template <typename Subject>
   inline bool modal_element<Subject>::click(context const& ctx, mouse_button btn)
   {
      this->subject().click(ctx, btn);
      return true;
   }

   template <typename Subject>
   inline bool modal_element<Subject>::key(context const& ctx, key_info k)
   {
      base_type::key(ctx, k);
      return true;
   }

   template <typename Subject>
   inline bool modal_element<Subject>::text(context const& ctx, text_info info)
   {
      base_type::text(ctx, info);
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////
   // An element that prevents any event from passing through. Add this as a
   // topmost layer in a view to lock the UI.
   ////////////////////////////////////////////////////////////////////////////
   inline auto ui_block(color color_ = {0.0, 0.0, 0.0, 0.5})
   {
      return modal(box_element{color_});
   }
}}

#endif
