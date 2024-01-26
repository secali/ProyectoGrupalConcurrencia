   // Función lambda para fusionar un mapa parcial generado con generate_map
   // dentro del mapa principal processed_data
   auto process_map = [&](map_type const& partial_map) -> void {
      for (auto const& [ext, info] : partial_map) {
         processed_data[ext].num_files += info.num_files;
         processed_data[ext].total_size += info.total_size;
      }
   };