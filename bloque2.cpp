// Calculamos el tamaño de cada chunk y el residuo para el último chunk
auto const chunk_size = paths.size() / num_futures;
auto const residual_chunk = paths.size() % num_futures;

// Inicializamos un iterador al comienzo del vector de rutas
auto iter = paths.begin();

// Lanzamos num_futures hilos adicionales para analizar los bloques
for (std::size_t i = 0; i < num_futures; ++i) {
    // Calculamos el tamaño del chunk para este hilo
    auto const chunk_sz = (i == num_futures - 1) ? chunk_size + residual_chunk : chunk_size;

    // Creamos un future que ejecuta generate_map en un chunk del vector de rutas
    futures.push_back(std::async(std::launch::async, generate_map, std::vector<fs::path>(iter, iter + chunk_sz)));

    // Avanzamos el iterador al siguiente chunk
    std::advance(iter, chunk_sz);
}

// Main analiza el trozo remanente del vector de rutas también con generate_map
auto const remaining_chunk = std::vector<fs::path>(iter, paths.end());
auto const main_result = generate_map(remaining_chunk);

// Aseguramos que todos los hilos hayan terminado antes de continuar
for (auto& future : futures) {
    auto result = future.get();
    process_map(result);
}

// Procesamos el resultado de main
process_map(main_result);
