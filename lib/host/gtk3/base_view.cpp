/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements/app.hpp>
#include <cairo.h>
#include <infra/assert.hpp>
#include <elements/base_view.hpp>
#include <elements/window.hpp>
#include <elements/support/canvas.hpp>
#include <elements/support/resource_paths.hpp>
#include <elements/support/font.hpp>
#include <elements/support/text_utils.hpp>
#include <gtk/gtk.h>
#include <map>
#include <string>

namespace cycfi::elements
{
   struct host_view
   {
      host_view();
      ~host_view();

      cairo_surface_t* surface = nullptr;
      GtkWidget* widget = nullptr;

      // Mouse button click tracking
      std::uint32_t click_time = 0;
      std::uint32_t click_count = 0;

      // Scroll acceleration tracking
      std::uint32_t scroll_time = 0;

      point cursor_position;

      using key_map = std::map<key_code, key_action>;
      key_map keys;

      int modifiers = 0; // the latest modifiers

      GtkIMContext* im_context;

      GdkCursorType active_cursor_type = GDK_ARROW;
      point                      _size;               // The current view size
      std::unique_ptr<drop_info> _drop_info;          // For drag and drop
   };

   struct platform_access
   {
      inline static host_view* get_host_view(base_view& view)
      {
         return view.host();
      }
   };

   host_view::host_view()
    : im_context(gtk_im_context_simple_new())
   {
   }

   host_view::~host_view()
   {
      if (surface)
         cairo_surface_destroy(surface);
      surface = nullptr;
   }

   namespace
   {
      // Some globals
      host_view* host_view_under_cursor = nullptr;
      GdkCursorType view_cursor_type = GDK_ARROW;

      inline base_view& get(gpointer user_data)
      {
         return *reinterpret_cast<base_view*>(user_data);
      }

      gboolean on_configure(GtkWidget* widget, GdkEventConfigure* /* event */, gpointer user_data)
      {
         auto& view = get(user_data);
         auto* host_view_h = platform_access::get_host_view(view);

         if (host_view_h->surface)
            cairo_surface_destroy(host_view_h->surface);

         host_view_h->surface = gdk_window_create_similar_surface(
            gtk_widget_get_window(widget), CAIRO_CONTENT_COLOR,
            gtk_widget_get_allocated_width(widget),
            gtk_widget_get_allocated_height(widget)
         );
         return true;
      }

      gboolean on_draw(GtkWidget* widget, cairo_t* cr, gpointer user_data)
      {
         auto& view = get(user_data);
         auto* host_view_h = platform_access::get_host_view(view);
         cairo_set_source_surface(cr, host_view_h->surface, 0, 0);
         cairo_paint(cr);

         // Note that cr (cairo_t) is already clipped to only draw the exposed
         // areas of the widget. double left, top, right, bottom;
         view.draw(cr);

         return false;
      }

      template <typename Event>
      bool get_mouse(Event* event, mouse_button& btn, host_view* view)
      {
         btn.modifiers = 0;
         if (event->state & GDK_SHIFT_MASK)
            btn.modifiers |= mod_shift;
         if (event->state & GDK_CONTROL_MASK)
            btn.modifiers |= mod_control | mod_action;
         if (event->state & GDK_MOD1_MASK)
            btn.modifiers |= mod_alt;
         if (event->state & GDK_SUPER_MASK)
            btn.modifiers |= mod_action;

         btn.num_clicks = view->click_count;
         btn.pos = {float(event->x), float(event->y)};
         return true;
      }

      bool get_button(GdkEventButton* event, mouse_button& btn, host_view* view)
      {
         if (event->button > 4)
            return false;

         gint dbl_click_time;
         g_object_get(
            gtk_settings_get_default()
          , "gtk-double-click-time", &dbl_click_time
          , nullptr
         );

         switch (event->type)
         {
            case GDK_BUTTON_PRESS:
               btn.down = true;
               if ((event->time - view->click_time) < guint32(dbl_click_time))
                  ++view->click_count;
               else
                  view->click_count = 1;
               view->click_time = event->time;
               break;

            case GDK_BUTTON_RELEASE:
               btn.down = false;
               break;

            default:
               return false;
         }

         btn.state = mouse_button::what(event->button-1);

         if (!get_mouse(event, btn, view))
            return false;
         return true;
      }

      gboolean on_button(GtkWidget* /* widget */, GdkEventButton* event, gpointer user_data)
      {
         auto& view = get(user_data);
         mouse_button btn;
         if (get_button(event, btn, platform_access::get_host_view(view)))
            view.click(btn);
         return true;
      }

      gboolean on_motion(GtkWidget* /* widget */, GdkEventMotion* event, gpointer user_data)
      {
         auto& base_view = get(user_data);
         host_view* view = platform_access::get_host_view(base_view);
         mouse_button btn;
         if (get_mouse(event, btn, view))
         {
            view->cursor_position = btn.pos;

            if (event->state & GDK_BUTTON1_MASK)
            {
               btn.down = true;
               btn.state = mouse_button::left;
            }
            else if (event->state & GDK_BUTTON2_MASK)
            {
               btn.down = true;
               btn.state = mouse_button::middle;
            }
            else if (event->state & GDK_BUTTON3_MASK)
            {
               btn.down = true;
               btn.state = mouse_button::right;
            }
            else
            {
               btn.down = false;
            }

            if (btn.down)
               base_view.drag(btn);
            else
               base_view.cursor(view->cursor_position, cursor_tracking::hovering);
         }
         return true;
      }

      gboolean on_scroll(GtkWidget* /* widget */, GdkEventScroll* event, gpointer user_data)
      {
         auto& base_view = get(user_data);
         auto* host_view_h = platform_access::get_host_view(base_view);
         auto elapsed = std::max<float>(10.0f, event->time - host_view_h->scroll_time);
         static constexpr float _1s = 100;
         host_view_h->scroll_time = event->time;
         static constexpr float smooth_speed = 10.0f;

         float dx = 0;
         float dy = 0;
         float step = _1s / elapsed;

         switch (event->direction)
         {
            case GDK_SCROLL_UP:
               dy = step;
               break;
            case GDK_SCROLL_DOWN:
               dy = -step;
               break;
            case GDK_SCROLL_LEFT:
               dx = step;
               break;
            case GDK_SCROLL_RIGHT:
               dx = -step;
               break;
            case GDK_SCROLL_SMOOTH:
               dx = -event->delta_x * smooth_speed;
               dy = -event->delta_y * smooth_speed;
               break;
            default:
               break;
         }

         base_view.scroll(
            {dx, dy},
            {float(event->x), float(event->y)}
         );
         return true;
      }
   }

   static void change_window_cursor(GtkWidget* widget, GdkCursorType type)
   {
      GdkDisplay* display = gtk_widget_get_display(widget);
      GdkWindow* window = gtk_widget_get_window(widget);
      GdkCursor* cursor = gdk_cursor_new_for_display(display, type);
      gdk_window_set_cursor(window, cursor);
      g_object_unref(cursor);
   }

   gboolean on_event_crossing(GtkWidget* widget, GdkEventCrossing* event, gpointer user_data)
   {
      auto& base_view = get(user_data);
      auto* host_view_h = platform_access::get_host_view(base_view);
      host_view_h->cursor_position = point{float(event->x), float(event->y)};
      if (event->type == GDK_ENTER_NOTIFY)
      {
         base_view.cursor(host_view_h->cursor_position, cursor_tracking::entering);
         host_view_under_cursor = host_view_h;
         if (host_view_h->active_cursor_type != view_cursor_type)
         {
            change_window_cursor(widget, view_cursor_type);
            host_view_h->active_cursor_type = view_cursor_type;
         }
      }
      else
      {
         base_view.cursor(host_view_h->cursor_position, cursor_tracking::leaving);
         host_view_under_cursor = nullptr;
      }
      return true;
   }

   // Defined in key.cpp
   key_code translate_key(unsigned key);

   static void on_text_entry(GtkIMContext* /* context */, const gchar* str, gpointer user_data)
   {
      auto& base_view = get(user_data);
      auto* host_view_h = platform_access::get_host_view(base_view);
      auto cp = codepoint(str);
      base_view.text({cp, host_view_h->modifiers});
   }

   int get_mods(int state)
   {
      int mods = 0;
      if (state & GDK_SHIFT_MASK)
         mods |= mod_shift;
      if (state & GDK_CONTROL_MASK)
         mods |= mod_control | mod_action;
      if (state & GDK_MOD1_MASK)
         mods |= mod_alt;
      if (state & GDK_SUPER_MASK)
         mods |= mod_super;

      return mods;
   }

   void handle_key(base_view& _view, host_view::key_map& keys, key_info k)
   {
      bool repeated = false;

      if (k.action == key_action::release)
      {
         keys.erase(k.key);
         return;
      }

      if (k.action == key_action::press
         && keys[k.key] == key_action::press)
         repeated = true;

      keys[k.key] = k.action;

      if (repeated)
         k.action = key_action::repeat;

      _view.key(k);
   }

   gboolean on_key(GtkWidget* widget, GdkEventKey* event, gpointer user_data)
   {
      auto& base_view = get(user_data);
      auto* host_view_h = platform_access::get_host_view(base_view);
      gtk_im_context_filter_keypress(host_view_h->im_context, event);

      int modifiers = get_mods(event->state);
      auto const action = event->type == GDK_KEY_PRESS? key_action::press : key_action::release;
      host_view_h->modifiers = modifiers;

      // We don't want the shift key handled when obtaining the keyval,
      // so we do this again here, instead of relying on event->keyval
      guint keyval = 0;
      gdk_keymap_translate_keyboard_state(
         gdk_keymap_get_for_display(gtk_widget_get_display(widget)),
         event->hardware_keycode,
         GdkModifierType(event->state & ~GDK_SHIFT_MASK),
         event->group,
         &keyval,
         nullptr, nullptr, nullptr);

      auto const key = translate_key(keyval);
      if (key == key_code::unknown)
         return false;

      handle_key(base_view, host_view_h->keys, {key, action, modifiers});
      return true;
   }

   void on_focus(GtkWidget* /* widget */, GdkEventFocus* event, gpointer user_data)
   {
      auto& base_view = get(user_data);
      if (event->in)
         base_view.begin_focus();
      else
         base_view.end_focus();
   }

   int poll_function(gpointer user_data)
   {
      auto& base_view = get(user_data);
      base_view.poll();
      return true;
   }

   gboolean on_drag_motion(GtkWidget* /* widget */, GdkDragContext* context, gint x, gint y, guint time, gpointer user_data)
   {
      auto& base_view = get(user_data);
      auto* host_view_h = platform_access::get_host_view(base_view);

      if (!host_view_h->_drop_info)
      {
         host_view_h->_drop_info = std::make_unique<drop_info>();
         host_view_h->_drop_info->data["text/uri-list"] = ""; // We do not have the data yet
         host_view_h->_drop_info->where = point{float(x), float(y)};
         base_view.track_drop(*host_view_h->_drop_info, cursor_tracking::entering);
      }
      else
      {
         base_view.track_drop(*host_view_h->_drop_info, cursor_tracking::hovering);
         host_view_h->_drop_info->where = point{float(x), float(y)};
      }
      host_view_h->cursor_position = point{float(x), float(y)};

      // You can set the drag context to indicate whether the drop is accepted or not
      gdk_drag_status(context, GDK_ACTION_COPY, time);
      return true;
   }

   void on_drag_leave(GtkWidget* /* widget */, GdkDragContext* /* context */, guint /* time */, gpointer user_data)
   {
      auto& base_view = get(user_data);
      auto* host_view_h = platform_access::get_host_view(base_view);
      if (host_view_h->_drop_info)
         base_view.track_drop(*host_view_h->_drop_info, cursor_tracking::leaving);
   }

   void on_drag_data_received(
      GtkWidget* /* widget */, GdkDragContext* context, gint /* x */, gint /* y */
    , GtkSelectionData* data, guint info, guint time, gpointer user_data)
   {
      auto& base_view = get(user_data);
      auto* host_view_h = platform_access::get_host_view(base_view);
      bool success = false;

      if (host_view_h->_drop_info)
      {
         auto& base_view = get(user_data);
         auto* host_view_h = platform_access::get_host_view(base_view);

         if (info == 0)
         {
            std::string paths;
            gchar** uris = gtk_selection_data_get_uris(data);
            for (gchar** i = uris; i && *i; ++i)
            {
               if (!paths.empty())
                  paths += "\n";
               paths += *i;
            }

            host_view_h->_drop_info->data["text/uri-list"] = paths;
            success = base_view.drop(*host_view_h->_drop_info);
            g_strfreev(uris);
         }

         host_view_h->_drop_info.reset();
      }
      gtk_drag_finish(context, success, false, time);
   }

   GtkWidget* make_view(base_view& view, GtkWidget* parent)
   {
      auto* content_view = gtk_drawing_area_new();

      gtk_container_add(GTK_CONTAINER(parent), content_view);

      // Subscribe to content_view events
      g_signal_connect(content_view, "configure-event",
         G_CALLBACK(on_configure), &view);
      g_signal_connect(content_view, "draw",
         G_CALLBACK(on_draw), &view);
      g_signal_connect(content_view, "button-press-event",
         G_CALLBACK(on_button), &view);
      g_signal_connect (content_view, "button-release-event",
         G_CALLBACK(on_button), &view);
      g_signal_connect(content_view, "motion-notify-event",
         G_CALLBACK(on_motion), &view);
      g_signal_connect(content_view, "scroll-event",
         G_CALLBACK(on_scroll), &view);
      g_signal_connect(content_view, "enter-notify-event",
         G_CALLBACK(on_event_crossing), &view);
      g_signal_connect(content_view, "leave-notify-event",
         G_CALLBACK(on_event_crossing), &view);

      g_signal_connect(content_view, "drag-motion",
         G_CALLBACK(on_drag_motion), &view);
      g_signal_connect(content_view, "drag-leave",
         G_CALLBACK(on_drag_leave), &view);
      g_signal_connect(content_view, "drag-data-received",
         G_CALLBACK(on_drag_data_received), &view);

      // Enable drag-and-drop for uri-list(s)
      static GtkTargetEntry target_entries[] =
      {
         {const_cast<char*>("text/uri-list"), 0, 0}
      };

      gtk_drag_dest_set(content_view, GTK_DEST_DEFAULT_ALL,
         target_entries, G_N_ELEMENTS(target_entries), GDK_ACTION_COPY);

      gtk_widget_set_events(content_view,
         gtk_widget_get_events(content_view)
         | GDK_BUTTON_PRESS_MASK
         | GDK_BUTTON_RELEASE_MASK
         | GDK_POINTER_MOTION_MASK
         | GDK_SCROLL_MASK
         | GDK_ENTER_NOTIFY_MASK
         | GDK_LEAVE_NOTIFY_MASK
         | GDK_SMOOTH_SCROLL_MASK
      );

      // Subscribe to parent events
      g_signal_connect(parent, "key-press-event",
         G_CALLBACK(on_key), &view);
      g_signal_connect(parent, "key-release-event",
         G_CALLBACK(on_key), &view);
      g_signal_connect(parent, "focus-in-event",
         G_CALLBACK(on_focus), &view);
      g_signal_connect(parent, "focus-out-event",
         G_CALLBACK(on_focus), &view);

      gtk_widget_set_events(parent,
         gtk_widget_get_events(parent)
         | GDK_KEY_PRESS_MASK
         | GDK_FOCUS_CHANGE_MASK
      );

      // Subscribe to text entry commit
      g_signal_connect(view.host()->im_context, "commit",
         G_CALLBACK(on_text_entry), &view);

      // Create 1ms timer
      g_timeout_add(1, poll_function, &view);

      return content_view;
   }

   // Defined in window.cpp
   GtkWidget* get_window(host_window& h);
   void on_window_activate(host_window& h, std::function<void()> f);

   // Defined in app.cpp
   bool app_is_activated();

   std::filesystem::path get_app_path()
   {
      char result[PATH_MAX];
      // get the full path to the executable file of the current process.
      if (auto count = readlink("/proc/self/exe", result, PATH_MAX); count != -1)
      {
         result[count] = '\0'; // null terminate the string because readlink doesn't
         return std::filesystem::path(result);
      }
      throw std::runtime_error("Fatal Error: Failed to get application path");
   }

   fs::path find_resources()
   {
      fs::path const app_path = get_app_path();
      fs::path const app_dir = app_path.parent_path();

      if (app_dir.filename() == "bin")
      {
         fs::path path = app_dir.parent_path() / "share" / app_path.filename() / "resources";
         if (fs::is_directory(path))
            return path;
      }

      const fs::path app_resources_dir = app_dir / "resources";
      if (fs::is_directory(app_resources_dir))
         return app_resources_dir;

      return fs::current_path() / "resources";
   }

   struct init_view_class
   {
      init_view_class()
      {
         auto resource_path = find_resources();
         add_search_path(resource_path);
         font_paths().push_back(resource_path);
      }
   };

   base_view::base_view(extent /* size_ */)
    : base_view(new host_view)
   {
      // $$$ FIXME: Implement Me $$$
      CYCFI_ASSERT(false, "Unimplemented");
   }

   base_view::base_view(host_view_handle h)
    : _view(h)
   {
      static init_view_class init;
   }

   base_view::base_view(host_window_handle h)
    : base_view(new host_view)
   {
      auto make_view =
         [this, h]()
         {
            _view->widget = elements::make_view(*this, get_window(*h));
         };

      if (app_is_activated())
         make_view();
      else
         on_window_activate(*h, make_view);
   }

   base_view::~base_view()
   {
      if (host_view_under_cursor == _view)
         host_view_under_cursor = nullptr;
      delete _view;
   }

   point base_view::cursor_pos() const
   {
      return _view->cursor_position;
   }

   elements::extent base_view::size() const
   {
      auto x = gtk_widget_get_allocated_width(_view->widget);
      auto y = gtk_widget_get_allocated_height(_view->widget);
      return {float(x), float(y)};
   }

   void base_view::size(elements::extent p)
   {
      // $$$ Wrong: don't size the window!!! $$$
      gtk_window_resize(GTK_WINDOW(_view->widget), p.x, p.y);
   }

   void base_view::refresh()
   {
      GtkAllocation alloc;
      gtk_widget_get_allocation(_view->widget, &alloc);
      refresh({
         float(alloc.x),
         float(alloc.y),
         float(alloc.x + alloc.width),
         float(alloc.y + alloc.height)
      });
   }

   void base_view::refresh(rect area)
   {
      // Note: GTK uses int coordinates. Make sure area is not empty
      // when converting from float to int.
      gtk_widget_queue_draw_area(_view->widget,
         std::floor(area.left),
         std::floor(area.top),
         std::max<float>(area.width(), 1),
         std::max<float>(area.height(), 1)
      );
   }

   std::string clipboard()
   {
      GtkClipboard* clip = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
      gchar* text = gtk_clipboard_wait_for_text(clip);
      return std::string(text);
   }

   void clipboard(std::string const& text)
   {
      GtkClipboard* clip = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
      gtk_clipboard_set_text(clip, text.data(), text.size());
   }

   void set_cursor(cursor_type type)
   {
      switch (type)
      {
         case cursor_type::arrow:
            view_cursor_type = GDK_ARROW;
            break;
         case cursor_type::ibeam:
            view_cursor_type = GDK_XTERM;
            break;
         case cursor_type::cross_hair:
            view_cursor_type = GDK_CROSSHAIR;
            break;
         case cursor_type::hand:
            view_cursor_type = GDK_HAND2;
            break;
         case cursor_type::h_resize:
            view_cursor_type = GDK_SB_H_DOUBLE_ARROW;
            break;
         case cursor_type::v_resize:
            view_cursor_type = GDK_SB_V_DOUBLE_ARROW;
            break;
      }

      auto* host_view_h = host_view_under_cursor;
      if (host_view_h && host_view_h->active_cursor_type != view_cursor_type)
      {
         change_window_cursor(host_view_under_cursor->widget, view_cursor_type);
         host_view_h->active_cursor_type = view_cursor_type;
      }
   }

   namespace
   {
      std::string exec(char const* cmd)
      {
         std::array<char, 128> buffer;
         std::string result;
         std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
         if (!pipe)
            throw std::runtime_error("popen() failed!");
         while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
            result += buffer.data();
         return result;
      }

      point get_scroll_direction()
      {
         std::string output = exec("gsettings get org.gnome.desktop.peripherals.touchpad natural-scroll");
         output.erase(remove(output.begin(), output.end(), '\n'), output.end());

         if (output == "true")
            return {+1.0f, +1.0f};  // Assuming positive for natural scrolling
         else
            return {-1.0f, -1.0f};  // Assuming negative for traditional scrolling
      }
   }

   point scroll_direction()
   {
      using namespace std::chrono;
      static auto last_call = steady_clock::now() - seconds(10);
      static point dir = get_scroll_direction(); // Initial call

      // In case the user changed the scroll direction settings, we will call
      // get_scroll_direction() if 10 seconds have passed since the last call.
      auto now = steady_clock::now();
      if (duration_cast<seconds>(now - last_call) >= seconds(10))
      {
         dir = get_scroll_direction(); // Update the direction if 10 seconds have passed
         last_call = now;              // Update the last call time
      }

      return dir;
   }

}

