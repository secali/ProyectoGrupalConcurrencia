// [[BLOQUE DE CÓDIGO 4 A IMPLEMENTAR POR EL EQUIPO DE TRABAJO]]
for (auto& fut : futures) {
    // Obtener el mapa parcial generado por el hilo
    map_type partial_map = fut.get();
    
    // Iterar sobre el mapa parcial y fusionar con el mapa principal
    for (const auto& [ext, info] : partial_map) {
        processed_data[ext].num_files += info.num_files;
        processed_data[ext].total_size += info.total_size;
    }
}

// [[BLOQUE DE CÓDIGO 5 A IMPLEMENTAR POR EL EQUIPO DE TRABAJO]]
for (const auto& [ext, info] : processed_data) {
    // Imprimir información para cada extensión
    pel::println("{:>15}: {:>8} files {:>16} bytes", ext, info.num_files, info.total_size);
    
    // Actualizar las variables totales
    root_file += info.num_files;
    root_size += info.total_size;
}
