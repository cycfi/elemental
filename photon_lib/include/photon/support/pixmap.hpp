/*=============================================================================
   Copyright (c) 2016-2018 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(PHOTON_GUI_LIB_PIXMAP_SEPTEMBER_5_2016)
#define PHOTON_GUI_LIB_PIXMAP_SEPTEMBER_5_2016

#include <vector>
#include <memory>
#include <cairo.h>
#include <photon/support/point.hpp>
#include <photon/support/exception.hpp>

namespace photon
{
   class canvas;

   ////////////////////////////////////////////////////////////////////////////
   // Pixmaps
   ////////////////////////////////////////////////////////////////////////////
   struct failed_to_load_pixmap : exception { using exception::exception; };

   class pixmap
   {
   public:

      explicit          pixmap(point size, float scale = 1);
      explicit          pixmap(char const* filename, float scale = 1);
                        pixmap(pixmap const& rhs) = delete;
                        pixmap(pixmap&& rhs);
                        ~pixmap();

      pixmap&           operator=(pixmap const& rhs) = delete;
      pixmap&           operator=(pixmap&& rhs);

      photon::size      size() const;
      float             scale() const;
      void              scale(float val);

   //private:

      friend class canvas;
      friend class pixmap_context;

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

                        pixmap_context(pixmap_context&& rhs)
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
      _surface = rhs._surface;
      rhs._surface = nullptr;
      return *this;
   }
}

#endif
