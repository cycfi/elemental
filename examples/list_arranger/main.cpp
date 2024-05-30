/*=============================================================================
   Copyright (c) 2022 Johann Philippe

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements.hpp>

using namespace cycfi::elements;
using cycfi::elements::rgba;

// Main window background
auto make_bkd()
{
   return port(image{"dark-bkd.jpg"});
}

// Probably shouldn't be global, but hey, it's a demo. You know what to do :-)
std::vector<std::filesystem::path> paths = {
   "a/home/user/documents/quantum_energy_matrix_42.txt",
   "b/var/www/html/interdimensional_portal_manifest.html",
   "c/usr/bin/elixir_of_eternal_life.exe",
   "d/mnt/data/enigmatic_astral_code_vortex.jpg",
   "e/opt/software/ancient_relics_archeology.ini",
   "f/tmp/temp/mystic_scroll_of_knowledge.tmp",
   "g/home/user/documents/hyperdimensional_cosmic_key_99.txt",
   "h/var/log/transcendental_being_encounter.log",
   "i/usr/lib/quantum_realm_gateway.so",
   "j/mnt/data/sacred_harmonic_resonance_music.mp3",
   "k/opt/software/ethereal_data_oracle.json",
   "l/tmp/temp/ancient_prophecy_tablet.zip",
   "m/home/user/documents/esoteric_mind_matrix.txt",
   "n/var/www/html/arcane_ritual_summoning_page.html",
   "o/usr/bin/ouroboros_eternal_loop_script.sh",
   "p/mnt/data/celestial_beauty_revelation_video.mp4",
   "q/opt/software/sacred_geometry_universe.cfg",
   "r/tmp/temp/profound_astral_chart.csv",
   "s/home/user/documents/elixir_of_infinite_wisdom.txt",
   "t/var/log/mystical_realm_access_error.log",
   "u/usr/bin/magical_portal_activation.exe",
   "v/mnt/data/cosmic_energy_matrix_manifest.jpg",
   "w/opt/software/ancient_tome_of_knowledge.log",
   "x/tmp/temp/akashic_records_of_creation.txt",
   "y/home/user/documents/quantum_cosmic_frequencies.txt",
   "z/var/www/html/ethereal_realm_connection.css",
   "/usr/bin/astral_projection_script.rb",
   "/mnt/data/ancient_chants_of_enlightenment_music.mp3",
   "/opt/software/celestial_beings_communication.png",
   "/tmp/temp/symbolic_matrix_of_destiny.txt",
   "/home/user/documents/mystical_arcane_encryption.txt",
   "/var/log/divine_knowledge_revelation.log",
   "/usr/bin/cosmic_energy_transmutation.exe",
   "/mnt/data/sacred_vortex_of_enlightenment.jpg",
   "/opt/software/quantum_leap_to_alternate_universes.ini",
   "/tmp/temp/astral_planetary_transmission.tmp",
   "/home/user/documents/mystical_aura_harmonization.txt",
   "/var/www/html/holistic_energy_alignment.html",
   "/usr/bin/sacred_geometric_energy_script.sh",
   "/mnt/data/ancient_sonic_resonance_journey_music.mp3",
   "/opt/software/interstellar_portal_blueprint.json",
   "/tmp/temp/holographic_mind_matrix.zip",
   "/home/user/documents/quantum_entanglement_manifest.txt",
   "/var/log/mystical_realm_entity_encounter.log",
   "/usr/bin/akashic_record_omniscience.exe",
   "/mnt/data/ethereal_harmonic_beauty.jpg",
   "/opt/software/holographic_realm_communication.cfg",
   "/tmp/temp/celestial_energy_nexus.csv",
   "/home/user/documents/universal_consciousness_download.txt",
   "/var/www/html/sacred_symbolic_vibration.css",
   "/usr/bin/mystical_encryption_script.rb",
   "/mnt/data/ancient_vibrational_mantras_music.mp3",
   "/opt/software/transdimensional_knowledge_portal.png",
   "/tmp/temp/mind_expansion_ritual.txt"
};

int main(int argc, char* argv[])
{
   app _app("Active Dynamic List");
   window _win(_app.name());
   _win.on_close = [&_app]() { _app.stop(); };

   view view_(_win);

   size_t list_size = paths.size();
   auto && make_row = [&](size_t index)
   {
      // If we start with an empty paths vector, we still need to give it a prototypical
      // element in order to establish the size limits.
      std::string path = paths.empty()? std::string{"Empty"} : paths[index].string();

      return share(
         draggable(
            align_left(label(path))
         )
      );
   };

   auto cp = basic_vcell_composer(list_size, make_row);
   auto list = vlist(cp, false);
   auto drop_inserter_ =
      share(
         drop_inserter(
            margin({10, 10, 15, 10}, link(list)),
            {"text/uri-list"}
         )
      );

   drop_inserter_->on_drop =
      [&](drop_info const& info, std::size_t ix)
      {
         if (contains_filepaths(info.data))
         {
            auto new_paths = get_filepaths(info.data);
            paths.insert(ix+paths.begin(), new_paths.begin(), new_paths.end());
            list.insert(ix, new_paths.size());
            view_.refresh();
            return true;
         }
         return false;
      };

   drop_inserter_->on_move =
      [&](std::size_t pos, std::vector<std::size_t> const& indices)
      {
         move_indices(paths, pos, indices);
         view_.refresh();
      };

   drop_inserter_->on_erase =
      [&](std::vector<std::size_t> const& indices)
      {
         erase_indices(paths, indices);
         view_.refresh();
      };

   view_.content(
      vscroller(align_left_top(hold(drop_inserter_))),
      make_bkd()
   );

   _app.run();
   return 0;
}
