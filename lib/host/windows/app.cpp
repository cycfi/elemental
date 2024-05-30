/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements/app.hpp>
#include <elements/support/font.hpp>
#include <infra/filesystem.hpp>
#include <windows.h>
#include <shlobj.h>
#include <cstring>
#include <ole2.h>

#ifndef ELEMENTS_HOST_ONLY_WIN7
#include <shellscalingapi.h>
#endif

namespace cycfi::elements
{
   app::app(std::string name)
   {
      _app_name = name;

#if !defined(ELEMENTS_HOST_ONLY_WIN7)
      SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
#endif

      OleInitialize(nullptr);
   }

   app::~app()
   {
   }

   void app::run()
   {
      MSG messages;
      while (_running && GetMessage(&messages, nullptr, 0, 0) > 0)
      {
         TranslateMessage(&messages);
         DispatchMessage(&messages);
      }
   }

   void app::stop()
   {
      _running = false;
      OleUninitialize();
   }

   fs::path app_data_path()
   {
      LPWSTR path = nullptr;
      SHGetKnownFolderPath(FOLDERID_ProgramData, KF_FLAG_CREATE, nullptr, &path);
      return fs::path{path};
   }
}

