   for (fs::path const& pth : chunk) {
         // para cada ruta pth en el bloque chunk, emplearemos las funciones
         // fs::is_regular_file y fs::is_directory para determinar si la ruta refiere
         // un fichero o un directorio, respectivamente. Si se trata de un fichero, emplearemos
         // su extensión pth.extension().string() y tamaño en memoria fs::file_size(pth)
         // para actualizar el mapa res. Si pth es es un directorio, incrementaremos el
         // contador atómico directory_counter en una unidad
         
         // [[BLOQUE DE CÓDIGO 1 A IMPLEMENTAR POR EL EQUIPO DE TRABAJO]]
		 // by FEC
		 if (fs::is_regular_file(pth)) {                        // verifica si la ruta
			std::string extension = pth.extension().string();   // Obtiene la extensión del archivo (pth.extension()) y la convierte a una cadena de caracteres
			std::uintmax_t file_size = fs::file_size(pth);      // Obtiene el tamaño en bytes del archivo referenciado por pth 
			res[extension].num_files++;							// Incrementa el contador de archivos 
			res[extension].total_size += file_size;             // Añade el tamaño del archivo al total acumulado para esa extensión en el mapa res.
		 } else if (fs::is_directory(pth)) {					// Verificamos si pth es un directorio 
			directory_counter.fetch_add(1, std::memory_order_relaxed);  //Incrementa de manera atómica el contador
         }// end by FEC
		 
      }

 
