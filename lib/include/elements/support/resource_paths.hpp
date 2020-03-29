/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_RESOURCE_PATHS_JUNE_22_2019)
#define ELEMENTS_RESOURCE_PATHS_JUNE_22_2019

#include <set>
#include <string_view>
#include <vector>
#include <filesystem>

namespace cycfi { namespace elements
{
   // Resources (e.g. images) that are identified by file names can be
   // absolute or relative paths. For relative paths, the resource_paths
   // vector below are used to search for such files, in the order they
   // appear in the vector. Platform code provides the initial paths.
   // Applications may add additional paths as needed.

  extern std::vector<std::filesystem::path> resource_paths;

   // Search for a file using the resource_paths. Returns an empty
   // string if file is not found.
   std::string find_file(std::string_view file);

   // Get the application data path
   std::filesystem::path app_data_path();
}}

#endif
