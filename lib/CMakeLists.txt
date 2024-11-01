###############################################################################
#  Copyright (c) 2016-2024 Joel de Guzman
#  Copyright (c) 2020 Michał Urbański
#
#  Distributed under the MIT License (https://opensource.org/licenses/MIT)
###############################################################################
cmake_minimum_required(VERSION 3.16.0)

include(FetchContent)

###############################################################################
# Sources (and Resources)

set(ELEMENTS_SOURCES
   src/element/button.cpp
   src/element/child_window.cpp
   src/element/composite.cpp
   src/element/dial.cpp
   src/element/drag_and_drop.cpp
   src/element/element.cpp
   src/element/floating.cpp
   src/element/flow.cpp
   src/element/grid.cpp
   src/element/image.cpp
   src/element/label.cpp
   src/element/layer.cpp
   src/element/list.cpp
   src/element/menu.cpp
   src/element/popup.cpp
   src/element/port.cpp
   src/element/proxy.cpp
   src/element/range_slider.cpp
   src/element/selection.cpp
   src/element/slider.cpp
   src/element/status_bar.cpp
   src/element/style/border.cpp
   src/element/style/button.cpp
   src/element/style/check_box.cpp
   src/element/style/grid_lines.cpp
   src/element/style/icon_button.cpp
   src/element/style/menu.cpp
   src/element/style/misc.cpp
   src/element/style/radio_button.cpp
   src/element/style/slide_switch.cpp
   src/element/style/slider.cpp
   src/element/style/thumbwheel.cpp
   src/element/text.cpp
   src/element/thumbwheel.cpp
   src/element/tile.cpp
   src/element/tooltip.cpp
   src/support/canvas.cpp
   src/support/draw_utils.cpp
   src/support/font.cpp
   src/support/glyphs.cpp
   src/support/pixmap.cpp
   src/support/receiver.cpp
   src/support/rect.cpp
   src/support/text_utils.cpp
   src/support/resource_paths.cpp
   src/support/text_utils.cpp
   src/support/theme.cpp
   src/support/payload.cpp
   src/support/receiver.cpp
   src/support/text_utils.cpp
   src/support/theme.cpp
   src/view.cpp
)

set(ELEMENTS_HEADERS
   include/elements.hpp
   include/elements/app.hpp
   include/elements/base_view.hpp
   include/elements/element.hpp
   include/elements/element/align.hpp
   include/elements/element/button.hpp
   include/elements/element/collapsable.hpp
   include/elements/element/composite.hpp
   include/elements/element/dial.hpp
   include/elements/element/drag_and_drop.hpp
   include/elements/element/element.hpp
   include/elements/element/events_intercept.hpp
   include/elements/element/floating.hpp
   include/elements/element/flow.hpp
   include/elements/element/grid.hpp
   include/elements/element/image.hpp
   include/elements/element/indirect.hpp
   include/elements/element/hidable.hpp
   include/elements/element/label.hpp
   include/elements/element/layer.hpp
   include/elements/element/list.hpp
   include/elements/element/margin.hpp
   include/elements/element/menu.hpp
   include/elements/element/modal.hpp
   include/elements/element/popup.hpp
   include/elements/element/port.hpp
   include/elements/element/proxy.hpp
   include/elements/element/range_slider.hpp
   include/elements/element/selection.hpp
   include/elements/element/size.hpp
   include/elements/element/slider.hpp
   include/elements/element/status_bar.hpp
   include/elements/element/style.hpp
   include/elements/element/style/border.hpp
   include/elements/element/style/button.hpp
   include/elements/element/style/caption.hpp
   include/elements/element/style/check_box.hpp
   include/elements/element/style/dialog.hpp
   include/elements/element/style/grid_lines.hpp
   include/elements/element/style/group.hpp
   include/elements/element/style/icon_button.hpp
   include/elements/element/style/menu.hpp
   include/elements/element/style/message_box.hpp
   include/elements/element/style/misc.hpp
   include/elements/element/style/notebook.hpp
   include/elements/element/style/pane.hpp
   include/elements/element/style/radio_button.hpp
   include/elements/element/style/tab.hpp
   include/elements/element/style/text_entry.hpp
   include/elements/element/style/thumbwheel.hpp
   include/elements/element/text.hpp
   include/elements/element/thumbwheel.hpp
   include/elements/element/tile.hpp
   include/elements/element/tracker.hpp
   include/elements/support.hpp
   include/elements/support/canvas.hpp
   include/elements/support/circle.hpp
   include/elements/support/color.hpp
   include/elements/support/context.hpp
   include/elements/support/detail/canvas_impl.hpp
   include/elements/support/detail/scratch_context.hpp
   include/elements/support/detail/stb_image.h
   include/elements/support/draw_utils.hpp
   include/elements/support/font.hpp
   include/elements/support/glyphs.hpp
   include/elements/support/icon_ids.hpp
   include/elements/support/pixmap.hpp
   include/elements/support/point.hpp
   include/elements/support/receiver.hpp
   include/elements/support/rect.hpp
   include/elements/support/resource_paths.hpp
   include/elements/support/text_utils.hpp
   include/elements/support/theme.hpp
   include/elements/view.hpp
   include/elements/window.hpp
)

if (APPLE)
   set(ELEMENTS_HOST
      host/macos/app.mm
      host/macos/base_view.mm
      host/macos/key.mm
      host/macos/window.mm
   )
elseif (ELEMENTS_HOST_UI_LIBRARY STREQUAL "gtk")
   set(ELEMENTS_HOST
      host/gtk3/app.cpp
      host/gtk3/base_view.cpp
      host/gtk3/key.cpp
      host/gtk3/window.cpp
   )
elseif (WIN32)
   set(ELEMENTS_HOST
      host/windows/utils.hpp
      host/windows/app.cpp
      host/windows/base_view.cpp
      host/windows/key.cpp
      host/windows/window.cpp
      host/windows/drag_and_drop.cpp
   )
endif()

source_group("Source Files\\elements"
   FILES
   ${ELEMENTS_SOURCES}
)

source_group("Source Files\\elements_host"
   FILES
   ${ELEMENTS_HOST}
)

source_group("Header Files\\elements"
   FILES
   ${ELEMENTS_HEADERS}
)

add_library(elements STATIC)

target_sources(elements
   PRIVATE ${ELEMENTS_SOURCES} ${ELEMENTS_HOST}
   PUBLIC ${ELEMENTS_HEADERS}
)

target_include_directories(elements PUBLIC include)
target_compile_features(elements PUBLIC cxx_std_20)

if(ELEMENTS_ENABLE_LTO)
   set_target_properties(elements PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

###############################################################################
# PkgConfig

find_package(PkgConfig REQUIRED)

###############################################################################
# Get rid of certain warnings, and make msvc support utf-8 without adding bom

target_compile_options(elements PRIVATE
   $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic -ftemplate-backtrace-limit=0>
   $<$<CXX_COMPILER_ID:Clang>:-Wall -Wpedantic -ftemplate-backtrace-limit=0>
   $<$<CXX_COMPILER_ID:AppleClang>:-Wall -Wpedantic -ftemplate-backtrace-limit=0>
   $<$<CXX_COMPILER_ID:MSVC>:/W4 /wd4244 /wd4305 /wd4996 /wd4267 /wd4018 /wd4456 /utf-8>
)

if (MSVC)
   # Kinda silly to have to do this: https://bit.ly/2ZXjzzn
   STRING(REGEX REPLACE "/W3" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif()

if (APPLE)
   # Not sure why APPLE is not picking the target_compile_options above
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -ftemplate-backtrace-limit=0")
endif()

###############################################################################
# Cycfi Infra

FetchContent_Declare(
  cycfi_infra
  GIT_REPOSITORY https://github.com/cycfi/infra.git
  GIT_TAG master
  GIT_SUBMODULES_RECURSE ON
)

FetchContent_MakeAvailable(cycfi_infra)
target_link_libraries(elements PUBLIC cycfi::infra)

###############################################################################
# Cairo

pkg_check_modules(cairo REQUIRED IMPORTED_TARGET cairo)
target_link_libraries(elements PUBLIC PkgConfig::cairo)


###############################################################################
# Fontconfig

pkg_check_modules(fontconfig REQUIRED IMPORTED_TARGET fontconfig)
target_link_libraries(elements PUBLIC PkgConfig::fontconfig)

###############################################################################
# Freetype

pkg_check_modules(freetype2 REQUIRED IMPORTED_TARGET freetype2)
target_link_libraries(elements PUBLIC PkgConfig::freetype2)

###############################################################################
# Webp

pkg_check_modules(libwebp REQUIRED IMPORTED_TARGET libwebp)
target_link_libraries(elements PUBLIC PkgConfig::libwebp)

###############################################################################
# GTK3

if (ELEMENTS_HOST_UI_LIBRARY STREQUAL "gtk")
   pkg_check_modules(GTK3 REQUIRED IMPORTED_TARGET gtk+-3.0)
   target_link_libraries(elements PRIVATE PkgConfig::GTK3)
endif()

###############################################################################
# Asio headers

FetchContent_Declare(
  asio
  GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
  GIT_TAG asio-1-29-0
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(asio)

target_include_directories(elements PUBLIC ${asio_SOURCE_DIR}/asio/include)

###############################################################################
# Global options

if (APPLE)
   target_compile_definitions(elements PRIVATE
      ELEMENTS_CLASS_PREFIX=${ELEMENTS_CLASS_PREFIX}
   )
   target_compile_options(elements PUBLIC "-fobjc-arc")
elseif (WIN32)
   target_compile_definitions(elements PUBLIC
      WIN32_LEAN_AND_MEAN
      NOMINMAX
      _UNICODE
      BOOST_DATE_TIME_NO_LIB
      BOOST_REGEX_NO_LIB
   )

   if (CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
      set_property(TARGET elements PROPERTY
         MSVC_RUNTIME_LIBRARY "MultiThreadedDebug"
      )
   else()
      set_property(TARGET elements PROPERTY
         MSVC_RUNTIME_LIBRARY "MultiThreaded"
      )
   endif()

endif()

if(ELEMENTS_HOST_UI_LIBRARY STREQUAL "gtk")
    target_compile_definitions(elements PUBLIC ELEMENTS_HOST_UI_LIBRARY_GTK)
elseif(ELEMENTS_HOST_UI_LIBRARY STREQUAL "cocoa")
    if(NOT APPLE)
        message(FATAL_ERROR "Only macOS supports ELEMENTS_HOST_UI_LIBRARY=cocoa")
    endif()
    target_compile_definitions(elements PUBLIC ELEMENTS_HOST_UI_LIBRARY_COCOA)
elseif(ELEMENTS_HOST_UI_LIBRARY STREQUAL "win32")
    if(NOT WIN32)
        message(FATAL_ERROR "Only Windows supports ELEMENTS_HOST_UI_LIBRARY=win32")
    endif()
    target_compile_definitions(elements PUBLIC ELEMENTS_HOST_UI_LIBRARY_WIN32)
    if(ELEMENTS_HOST_ONLY_WIN7)
        target_compile_definitions(elements PRIVATE ELEMENTS_HOST_ONLY_WIN7)
        message(STATUS "Windows 7 compatibility enabled")
    elseif(MINGW)
        # Required by MinGW to enable SetProcessDpiAwareness (Windows 8.1+)
        # and GetDpiForWindow (Windows 10+) API
        target_compile_definitions(elements PRIVATE _WIN32_WINNT=0x0A00)
    endif()
else()
    message(FATAL_ERROR "Invalid ELEMENTS_HOST_UI_LIBRARY=${ELEMENTS_HOST_UI_LIBRARY}. Set gtk, cocoa or win32.")
endif()

###############################################################################
# Libraries and linking

if (UNIX OR (WIN32 AND NOT MSVC))
   find_package(Threads)
   target_link_libraries(elements PUBLIC Threads::Threads)
endif()

# ASIO requires this
if (WIN32 AND NOT MSVC)
   target_link_libraries(elements PUBLIC ws2_32)
endif()

# 1.Replace missing Shcore library removed by WIN32_LEAN_AND_MEAN
# 2.IME requires Imm32
if (WIN32)
   target_link_libraries(elements PUBLIC Shcore Imm32)
endif()

add_library(cycfi::elements ALIAS elements)
