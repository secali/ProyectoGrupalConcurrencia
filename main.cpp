#include <atomic>     // variable atómicas
#include <chrono>     // utilidades de fechas y hora
#include <filesystem> // sistema de ficheros
#include <future>     // incluye objetos futuros y std::async
#include <map>        // contenedor asociativo de clave ordenada única
#include <ranges>
#include <string>
#include <thread> // hilos
#include <vector> // contenedor secuencial de elementos contiguos

#include "pel_print.hpp"

namespace rn = std::ranges;     // alias
namespace fs = std::filesystem; // alias

struct Extension_info
{                            // información global acerca de una extensión determinada (.exe, .txt, etc)
   std::uintmax_t num_files, // número total de archivos con una extensión determinada
       total_size;           // tamaño total de bytes acumulado por dicha extensión
};

auto main() -> int
{
   // solicitamos al usuario que indique una ruta, que almacenamos en un string:
   pel::print("Please insert a root: ");
   auto root_str = std::string{};
   std::getline(std::cin, root_str);

   // inicializamos un objeto std::filesystem::path con dicha ruta:
   auto const root = fs::path{root_str};
   // comprobamos que se trate de un directorio:
   if (!fs::is_directory(root))
   {
      pel::print("you must indicate an actual directory");
      return 0;
   }

   // empezamos a cronometrar la ejecución del programa:
   using clock = std::chrono::steady_clock;
   auto const start = clock::now();

   // generamos un vector con todas las rutas (ya sean subdirectorios o ficheros)
   // contenidas en root:
   auto paths = std::vector<fs::path>{};
   for (fs::path pth : fs::recursive_directory_iterator{root})
   {
      paths.push_back(pth);
   }

   // mapa global que asocia a cada extensión el número total de archivos y el tamaño
   // total acumulado por ellos:
   using map_type = std::map<std::string, Extension_info>; // alias
   auto processed_data = map_type{};                       // inicializamos el mapa inicialmente vacío

   // contador atómico de subdirectorios dentro de la ruta raíz:
   auto directory_counter = std::atomic<std::uintmax_t>{0};

   // tarea para un hilo: generación de un mapa parcial a partir de un bloque
   //                     (chunk) del vector de rutas paths
   auto generate_map = [&](auto chunk) -> map_type
   {
      auto res = map_type{}; // mapa parcial inicialmente vacío
      for (fs::path const &pth : chunk)
      {
         // para cada ruta pth en el bloque chunk, emplearemos las funciones
         // fs::is_regular_file y fs::is_directory para determinar si la ruta refiere
         // un fichero o un directorio, respectivamente. Si se trata de un fichero, emplearemos
         // su extensión pth.extension().string() y tamaño en memoria fs::file_size(pth)
         // para actualizar el mapa res. Si pth es es un directorio, incrementaremos el
         // contador atómico directory_counter en una unidad

         // [[BLOQUE DE CÓDIGO 1 A IMPLEMENTAR POR EL EQUIPO DE TRABAJO]]
         // by FEC
         if (fs::is_regular_file(pth))
         {                                                    // verifica si la ruta
            std::string extension = pth.extension().string(); // Obtiene la extensión del archivo (pth.extension()) y la convierte a una cadena de caracteres
            std::uintmax_t file_size = fs::file_size(pth);    // Obtiene el tamaño en bytes del archivo referenciado por pth
            res[extension].num_files++;                       // Incrementa el contador de archivos
            res[extension].total_size += file_size;           // Añade el tamaño del archivo al total acumulado para esa extensión en el mapa res.
         }
         else if (fs::is_directory(pth))
         {                                                             // Verificamos si pth es un directorio
            directory_counter.fetch_add(1, std::memory_order_relaxed); // Incrementa de manera atómica el contador
         }                                                             // end by FEC
      }
      return res;
   };

   // número de hilos de hardware:
   auto const hardw_threads = std::thread::hardware_concurrency();
   // número de hilos a lanzar:
   auto const num_futures = hardw_threads - 1;
   // tamaño máximo de un bloque/chunk del vector de rutas:
   auto const max_chunk_sz = paths.size() / hardw_threads;

   // vector de objetos std::future inicialmente vacío:
   auto futures = std::vector<std::future<map_type>>{};

   // dividimos el vector y lanzamos num_futures hilos adicionales para analizar
   // los bloques. Nuestro propósito es ejecutar generate_map asíncronamente en
   // ellos; mientras se ejecutan en paralelo los hilos lanzados, main se encargará de
   // analizar el trozo remanente del vector de rutas también con generate_map:

   // [[BLOQUE DE CÓDIGO 2 A IMPLEMENTAR POR EL EQUIPO DE TRABAJO]]

   // --------------------------------------------------------------------------

   // función lambda para fusionar un mapa parcial generado con generate_map
   // dentro del mapa principal processed_data:
   auto process_map = [&](map_type const &partial_map) -> void
   {
      // Bloque 3: Fusión de mapas parciales
      for (auto const &[ext, info] : partial_map)
      {
         processed_data[ext].num_files += info.num_files;
         processed_data[ext].total_size += info.total_size;
      }
   };

   // empleando la función process_map, fusionamos el mapa generado por main y
   // los obtenidos a través de cada objeto futuro dentro de processed_date:

   // [[BLOQUE DE CÓDIGO 4 A IMPLEMENTAR POR EL EQUIPO DE TRABAJO]]
   for (auto &fut : futures)
   {
      // Obtener el mapa parcial generado por el hilo
      map_type partial_map = fut.get();

      // Iterar sobre el mapa parcial y fusionar con el mapa principal
      for (const auto &[ext, info] : partial_map)
      {
         processed_data[ext].num_files += info.num_files;
         processed_data[ext].total_size += info.total_size;
      }
   }
   // --------------------------------------------------------------------------

   // variables para acumular la información total del directorio root:
   auto root_file = std::uintmax_t{0}; // número total de archivos dentro de la ruta raíz
   auto root_size = std::uintmax_t{0}; // tamaño total en bytes de los archivos

   // imprimimos la información relevante para cada extensión (tipo de extensión,
   // número de archivos y tamaño acumulado por ellos), actualizando root_file y
   // root_size:
   for (const auto &[ext, info] : processed_data)
   {
      // Imprimir información para cada extensión
      pel::println("{:>15}: {:>8} files {:>16} bytes", ext, info.num_files, info.total_size);

      // Actualizar las variables totales
      root_file += info.num_files;
      root_size += info.total_size;
   }

   // imprimimos la información total del directorio y el tiempo de ejecución:
   auto const stop = clock::now();
   auto const duration = duration_cast<std::chrono::milliseconds>(stop - start);
   pel::println("\n{:>15}: {:>8} files {:>16} bytes | {} folders [{} ms]",
                "Total", root_file, root_size, directory_counter.load(), duration.count());
}