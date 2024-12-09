/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_PIXMAP_SEPTEMBER_5_2016)
#define ELEMENTS_PIXMAP_SEPTEMBER_5_2016

#include <vector>
#include <memory>
#include <cairo.h>
#include <elements/support/point.hpp>
#include <infra/filesystem.hpp>
#include <stdexcept>

namespace cycfi { namespace elements
{
   class canvas;

   ////////////////////////////////////////////////////////////////////////////
   // Pixmaps
   ////////////////////////////////////////////////////////////////////////////
   struct failed_to_load_pixmap : std::runtime_error
   {
      using std::runtime_error::runtime_error;
   };

   enum class pixel_format {
      invalid = -1,
      rgba32,
   };

   class pixmap
   {
   public:

      explicit          pixmap(point size, float scale = 1);
      explicit          pixmap(fs::path const& path, float scale = 1);
                        pixmap(pixmap const& rhs) = delete;
                        pixmap(pixmap&& rhs);
                        ~pixmap();

      pixmap&           operator=(pixmap const& rhs) = delete;
      pixmap&           operator=(pixmap&& rhs);

      extent            size() const;
      float             scale() const;
      void              scale(float val);

   private:

      friend class canvas;
      friend class pixmap_context;

      template <pixel_format fmt>
      friend typename std::enable_if<fmt == pixel_format::rgba32, pixmap>::type
      make_pixmap(std::uint8_t const *data, extent size, float scale);

      explicit pixmap(std::uint8_t const *data, pixel_format fmt, extent size,
                      float scale);

      cairo_surface_t*  _surface;
   };

   using pixmap_ptr = std::shared_ptr<pixmap>;

   ////////////////////////////////////////////////////////////////////////////
   // pixmap_context allows drawing into a pixmap
   ////////////////////////////////////////////////////////////////////////////
   class pixmap_context
   {
   public:

      explicit          pixmap_context(pixmap& pm)
                        {
                           _context = cairo_create(pm._surface);
                        }

                        ~pixmap_context()
                        {
                           if (_context)
                              cairo_destroy(_context);
                        }

                        pixmap_context(pixmap_context&& rhs) noexcept
                         : _context(rhs._context)
                        {
                           rhs._context = nullptr;
                        }

      cairo_t*          context() const { return _context; }

   private:
                        pixmap_context(pixmap_context const&) = delete;

      cairo_t*          _context;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Inlines
   ////////////////////////////////////////////////////////////////////////////
   inline pixmap::pixmap(pixmap&& rhs)
    : _surface(rhs._surface)
   {
      rhs._surface = nullptr;
   }

   inline pixmap& pixmap::operator=(pixmap&& rhs)
   {
      if (this != &rhs)
      {
        std::swap(_surface, rhs._surface);
      }
      return *this;
   }

   //--------------------------------------------------------------------------
   // Inlines
   //--------------------------------------------------------------------------

   template <pixel_format fmt>
   inline typename std::enable_if<fmt == pixel_format::rgba32, pixmap>::type
   make_pixmap(std::uint8_t const *data, extent size, float scale=1)
   {
      return pixmap(reinterpret_cast<std::uint8_t const *>(data), fmt, size, scale);
   }

   }}

#endif
