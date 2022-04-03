#include "myr_archive.hpp"
#include <filesystem>

namespace fs = std::filesystem;

int main(int argc, char *argv[]) {
  Myr::Archive myarc{};
  std::cout << Myr::xor_key << '\n' << Myr::xor_key_sz << '\n';
  auto input_dir = fs::path(argc > 1 ? argv[1] : ".");
  auto output_dir = fs::path(argc > 2 ? argv[2] : ".");
  for (auto &&file : fs::directory_iterator(input_dir)) {
    if (!file.is_regular_file())
      continue;
    std::cout << file.path() << '\n';
    try {
      myarc.add_entry(Myr::Entry(file, file.file_size()));
    } catch (const std::ios_base::failure &e) {
      std::cout << "Caught an ios_base::failure.\n"
                << "Explanatory string: " << e.what() << '\n'
                << "Error code: " << e.code() << '\n';
    }
  }
  output_dir /= "new_archive";
  myarc.try_export(output_dir.string());
  return 0;
}
