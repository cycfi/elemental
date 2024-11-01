/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_TEXT_APRIL_17_2016)
#define ELEMENTS_TEXT_APRIL_17_2016

#include <elements/support/glyphs.hpp>
#include <elements/support/theme.hpp>
#include <elements/element/element.hpp>

#include <infra/string_view.hpp>
#include <string>
#include <vector>

namespace cycfi::elements
{
   ////////////////////////////////////////////////////////////////////////////
   // text_reader and text_writer mixins
   ////////////////////////////////////////////////////////////////////////////
   class text_reader
   {
   public:

      virtual                    ~text_reader() = default;
      virtual std::string const& get_text() const = 0;
      char const*                c_str() const { return get_text().c_str(); }
   };

   class text_writer
   {
   public:

      virtual                    ~text_writer() = default;
      virtual void               set_text(string_view text) = 0;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Static Text Box
   ////////////////////////////////////////////////////////////////////////////
   class static_text_box
    : public element
    , public text_reader
    , public text_writer
    , public receiver<std::string>
   {
   public:
                              static_text_box(
                                 std::string text
                               , font font_        = get_theme().text_box_font
                               , color color_      = get_theme().text_box_font_color
                              );

                              static_text_box(static_text_box&& rhs) = default;

      view_limits             limits(basic_context const& ctx) const override;
      void                    layout(context const& ctx) override;
      void                    draw(context const& ctx) override;

      std::string const&      get_text() const override            { return _text; }
      void                    set_text(string_view text) override;
      std::string const&      get_utf8() const                    { return _text; }
      void                    set_utf8(string_view text)          { set_text(text); };

      std::string const&      value() const override           { return _text; }
      void                    value(string_view val) override;

      void                    set_color(color c)         { _color = c; }
      color                   get_color() const          { return _color; }
      point                   current_size() const       { return _current_size; };

   private:

      void                    sync() const;

   protected:

      std::string             _text;
      mutable master_glyphs   _layout;
      std::vector<glyphs>     _rows;
      color                   _color;
      point                   _current_size = {-1, -1};
   };

   ////////////////////////////////////////////////////////////////////////////
   // Editable Text Box
   ////////////////////////////////////////////////////////////////////////////
   class basic_text_box : public static_text_box
   {
   public:
                              basic_text_box(
                                 std::string text
                               , font font_ = get_theme().text_box_font
                              );
                              ~basic_text_box();
                              basic_text_box(basic_text_box&& rhs) = default;

      void                    draw(context const& ctx) override;
      bool                    click(context const& ctx, mouse_button btn) override;
      void                    drag(context const& ctx, mouse_button btn) override;
      bool                    cursor(context const& ctx, point p, cursor_tracking status) override;
      bool                    key(context const& ctx, key_info k) override;
      bool                    wants_focus() const override;
      void                    begin_focus(focus_request req) override;
      bool                    end_focus() override;
      bool                    wants_control() const override;

      bool                    text(context const& ctx, text_info info) override;
      void                    set_text(string_view text) override;

      using element::focus;
      using static_text_box::get_text;

      int                     select_start() const    { return _select_start; }
      void                    select_start(int pos);
      int                     select_end() const      { return _select_end; }
      void                    select_end(int pos);
      void                    select_all();
      void                    select_none();
      void                    scroll_into_view()      { _scroll_into_view = true; }

      void                    home(bool shift = false);
      void                    end(bool shift = false);

      virtual void            draw_selection(context const& ctx);
      virtual void            draw_caret(context const& ctx);
      virtual bool            word_break(char const* utf8) const;
      virtual bool            line_break(char const* utf8) const;

      bool                    is_focus() const              { return _is_focus; }
      basic_text_box&&        read_only() { _read_only = true; return std::move(*this); }
      void                    read_only(bool read_only_)    { _read_only = read_only_; }
      bool                    editable() const              { return !_read_only && _enabled; }

      basic_text_box&&        disable()   { _enabled = false; return std::move(*this); }
      basic_text_box&&        enable()    { _enabled = true; return std::move(*this); }
      bool                    is_enabled() const override   { return _enabled; };
      void                    enable(bool e) override       { _enabled = e; };

      void                    scroll_into_view(context const& ctx, bool save_x);
      virtual void            delete_(bool forward);
      virtual void            cut(view& v, int start, int end);
      virtual void            copy(view& v, int start, int end);
      virtual void            paste(view& v, int start, int end);

   protected:

      struct glyph_metrics
      {
         char const* str;           // The start of the utf8 string
         point       pos;           // Position where glyph is drawn
         rect        bounds;        // Glyph bounds
         float       line_height;   // Line height
      };

      char const*             caret_position(context const& ctx, point p);
      glyph_metrics           glyph_info(context const& ctx, char const* s);
      bool                    get_caret_position(context const& ctx, rect& caret_bounds);

   private:

      struct state_saver;
      using state_saver_f = std::function<void()>;

      state_saver_f           capture_state();

      using this_handle = std::shared_ptr<basic_text_box*>;
      using this_weak_handle = std::weak_ptr<basic_text_box*>;

      int                     _select_start;
      int                     _select_end;
      float                   _current_x;
      state_saver_f           _typing_state;
      bool                    _is_focus : 1;
      bool                    _show_caret : 1;
      bool                    _caret_started : 1;
      bool                    _read_only : 1;
      bool                    _enabled : 1;
      bool                    _scroll_into_view : 1;
      this_handle             _this_handle;
   };

   /**
    * \brief
    *    Represents a basic input box derived from the basic_text_box class,
    *    specialized for single-line text entry.
    *
    *    The `basic_input_box` class is a specialized version of the
    *    `basic_text_box` class. It inherits all the functionality of a
    *    `basic_text_box`, and adds support for placeholder text and text
    *    clipping actions.\n\n
    *
    *    The placeholder text is displayed when the input box is empty,
    *    providing a hint to the user about what they should enter. The text
    *    clipping action determines how the text is handled when it exceeds
    *    the box's width.\n\n
    *
    *    This class is specifically designed for single-line text entry,
    *    making it ideal for forms, search boxes, and other UI elements where
    *    the user needs to input a small amount of text.
    */
   class basic_input_box : public basic_text_box
   {
   public:

      using basic_text_box::get_text;

      using text_function = std::function<void(string_view text)>;
      using enter_function = std::function<bool(string_view text)>;
      using escape_function = std::function<void()>;

      enum clip_action { clip_none, clip_left, clip_right };

                              basic_input_box(
                                 std::string placeholder = ""
                               , font font_ = get_theme().text_box_font
                               , clip_action clip_action_ = clip_right
                              );

                              basic_input_box(
                                 std::string placeholder = ""
                               , clip_action clip_action_ = clip_right
                              );

                              basic_input_box(basic_input_box&& rhs) = default;

      view_limits             limits(basic_context const& ctx) const override;
      void                    draw(context const& ctx) override;
      bool                    text(context const& ctx, text_info info) override;
      bool                    key(context const& ctx, key_info k) override;
      void                    delete_(bool forward) override;

      bool                    click(context const& ctx, mouse_button btn) override;
      bool                    cursor(context const& ctx, point p, cursor_tracking status) override;
      bool                    scroll(context const& ctx, point dir, point p) override;
      bool                    end_focus() override;

      text_function           on_text;
      enter_function          on_enter;
      escape_function         on_escape;
      enter_function          on_end_focus;

   private:

      void                    paste(view& v, int start, int end) override;
      void                    make_maximally_visible(context const& ctx);
      void                    limit_scroll_right(context const& ctx);

      std::string             _placeholder;
      clip_action             _clip_action : 2;
      bool                    _is_hovering : 1;
   };

   //--------------------------------------------------------------------------
   // Inlines
   //--------------------------------------------------------------------------

   /**
    * \brief
    *    Constructs a basic_input_box with a placeholder, font, and clip
    *    action.
    *
    *    This constructor initializes a basic_input_box with a specified
    *    placeholder text, font, and clip action. The clip action determines
    *    how the text is clipped when it exceeds the box's width.
    *
    * \param placeholder
    *    The placeholder text to display when the input box is empty.
    *
    * \param font_
    *    The font to use for the text in the input box.
    *
    * \param clip_action_
    *    The action to take when the text exceeds the box's width.
    */
   inline basic_input_box::basic_input_box(
      std::string placeholder
    , font font_
    , clip_action clip_action_
   )
    : basic_text_box{"", font_}
    , _placeholder{std::move(placeholder)}
    , _clip_action{clip_action_}
   {}

   /**
    * \brief
    *    Constructs a basic_input_box with a placeholder and clip action.
    *
    *    This constructor initializes a basic_input_box with a specified
    *    placeholder text and clip action. The font is set to the default
    *    text box font from the current theme.
    *
    * \param placeholder
    *    The placeholder text to display when the input box is empty.
    *
    * \param clip_action_
    *    The action to take when the text exceeds the box's width.
    */
   inline basic_input_box::basic_input_box(
      std::string placeholder
    , clip_action clip_action_
   )
    : basic_input_box{
         placeholder
       , get_theme().text_box_font
       , clip_action_
      }
   {}
}

#endif
