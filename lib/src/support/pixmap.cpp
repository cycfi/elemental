/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/support/pixmap.hpp>
#include <elements/support/resource_paths.hpp>
#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_PNG 1
#include <webp/decode.h>
#include <elements/support/detail/stb_image.h>
#include <infra/assert.hpp>
#include <infra/filesystem.hpp>
#include <bit>
#include <string>
#include <fstream>

namespace cycfi { namespace elements
{
   pixmap::pixmap(point size, float scale)
    : _surface(cairo_image_surface_create(CAIRO_FORMAT_ARGB32, size.x, size.y))
   {
      if (!_surface)
         throw failed_to_load_pixmap{"Failed to create pixmap."};

      // Set scale and flag the surface as dirty
      cairo_surface_set_device_scale(_surface, 1/scale, 1/scale);
      cairo_surface_mark_dirty(_surface);
   }

   pixmap::pixmap(fs::path const& path, float scale)
    : _surface(nullptr)
   {
      auto ext = path.extension();
      if (ext.empty())
         throw failed_to_load_pixmap{"Unknown file type."};

      fs::path full_path = find_file(path);
      if (full_path.empty())
         throw failed_to_load_pixmap{"File does not exist."};

      uint8_t* src_data = nullptr;
      int w, h;

      if (ext == ".png" || ext == ".PNG")
      {
         // For PNGs, use Cairo's native PNG loader
         _surface = cairo_image_surface_create_from_png(full_path.string().c_str());
      }
      if (ext == ".webp" || ext == ".WEBP")
      {
         std::ifstream file(full_path.string().c_str(), std::ios::binary);
         if (!file.is_open())
            throw failed_to_load_pixmap{"Failed to open file: " + path.string()};

         // Get file size
         file.seekg(0, std::ios::end);
         std::streamsize size = file.tellg();
         file.seekg(0, std::ios::beg);

         // Read file into memory
         std::vector<uint8_t> data(size);
         if (!file.read((char*)data.data(), size))
            throw failed_to_load_pixmap{"Failed to read file: " + path.string()};

         src_data = WebPDecodeRGBA(data.data(), size, &w, &h);
      }
      else
      {
         // For everything else, use stb_image
         int components;
         src_data = stbi_load(full_path.string().c_str(), &w, &h, &components, 4);
      }

      if (src_data)
      {
         _surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, w, h);

         uint8_t* dest_data = cairo_image_surface_get_data(_surface);
         size_t   src_stride = w * 4;
         size_t   dest_stride = cairo_image_surface_get_stride(_surface);

         for (int y = 0; y != h; ++y)
         {
            uint8_t* src = src_data + (y * src_stride);
            uint8_t* dest = dest_data + (y * dest_stride);
            for (int x = 0; x != w; ++x)
            {
               // $$$ TODO: Deal with endian issues $$$

               dest[0] = src[2];    // blue
               dest[1] = src[1];    // green
               dest[2] = src[0];    // red
               dest[3] = src[3];    // alpha

               src += 4;
               dest += 4;
            }
         }

         free(src_data);
      }

      if (!_surface)
         throw failed_to_load_pixmap{"Failed to load pixmap."};

      // Set scale and flag the surface as dirty
      cairo_surface_set_device_scale(_surface, 1/scale, 1/scale);
      cairo_surface_mark_dirty(_surface);
   }

   pixmap::pixmap(uint8_t const *data, pixel_format fmt, extent size, float scale)
       : _surface(nullptr)
   {
      if (fmt == pixel_format::invalid)
         throw std::runtime_error{"Error: Cannot initialize format: INVALID"};

      if (fmt != pixel_format::rgba32)
         throw std::runtime_error{"Error: Format not supported"};

      _surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, size.x, size.y);
      if (!_surface)
         throw std::runtime_error{"Failed to create pixmap."};

      uint8_t *dest_data = cairo_image_surface_get_data(_surface);
      size_t src_stride = size.x * 4;
      size_t dest_stride = cairo_image_surface_get_stride(_surface);

      for (int y = 0; y != size.y; ++y)
      {
         uint8_t const *src = data + (y * src_stride);
         uint8_t *dest = dest_data + (y * dest_stride);
         for (int x = 0; x != size.x; ++x)
         {
            // RGBA32 to ARGB32
            if constexpr (std::endian::native == std::endian::big)
            {
               dest[0] = src[3]; // alpha
               dest[1] = src[0]; // red
               dest[2] = src[1]; // green
               dest[3] = src[2]; // blue
            }
            else if constexpr (std::endian::native == std::endian::little)
            {
               dest[0] = src[1]; // blue
               dest[1] = src[2]; // green
               dest[2] = src[3]; // red
               dest[3] = src[0]; // alpha
            }

            src += 4;
            dest += 4;
         }
      }

      // Set scale and flag the surface as dirty
      cairo_surface_set_device_scale(_surface, 1 / scale, 1 / scale);
      cairo_surface_mark_dirty(_surface);
   }

   pixmap::~pixmap()
   {
      if (_surface)
         cairo_surface_destroy(_surface);
   }

   extent pixmap::size() const
   {
      double scx, scy;
      cairo_surface_get_device_scale(_surface, &scx, &scy);
      return {
         float(cairo_image_surface_get_width(_surface) / scx),
         float(cairo_image_surface_get_height(_surface) / scy)
      };
   }

   float pixmap::scale() const
   {
      double scx, scy;
      cairo_surface_get_device_scale(_surface, &scx, &scy);
      return float(1/scx);
   }

   void pixmap::scale(float val)
   {
      cairo_surface_set_device_scale(_surface, 1/val, 1/val);
   }
}}
