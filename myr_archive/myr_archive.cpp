// myr_archive.cpp : Defines the entry point for the application.
//

#include "myr_archive.hpp"

namespace Myr {
Entry::Entry(const std::filesystem::path &s, std::size_t size)
    : path(s), filename(s.filename().string()), size(size), compression(){};

void Entry::set_filename(const std::string &str) { filename = str; }

auto Entry::get_size() const { return size; }

auto Entry::get_path() const { return path; }

auto Entry::get_compression() const { return compression; }

auto Entry::is_xor_enabled() const { return xor_encrypt; }

void Entry::set_compression(uint8_t level) { compression = level; }

void Entry::rem_compression() { compression.reset(); }

const std::string &Entry::get_filename() const { return filename; }

std::ostream &operator<<(std::ostream &os, const Entry &e) {
  constexpr auto buffer_size = 4096U;
  char buffer[buffer_size];

  std::ifstream inp(e.get_path(), std::ios::binary);

  std::size_t stream_size = e.get_size();

  auto encrypt = [&buffer](std::size_t buffer_size) {
    for (std::size_t i = 0u; i < buffer_size; ++i) {
      buffer[i] ^= xor_key[i % xor_key_sz];
    }
  };

  // Process buffer by buffer
  while (stream_size > buffer_size) {
    inp.read(buffer, buffer_size);
    if (e.is_xor_enabled()) {
      encrypt(buffer_size);
    }
    os.write(buffer, buffer_size);
    stream_size -= buffer_size;
#ifdef _DEBUG
    _ASSERTE(inp.good());
#endif
  }
  // Process the rest
  inp.read(buffer, stream_size);
  encrypt(stream_size);
  os.write(buffer, stream_size);

  return os;
}

void Archive::add_entry(const Entry &new_entry) {
  entries.insert({new_entry.get_filename(), new_entry});
}

Myr::Entry &Archive::get_entry(const MapKey &f) { return entries.at(f); }

void Archive::remove_entry(const MapKey &key) { entries.erase(key); }

void Archive::try_export(const std::string &export_name) {
  if (std::ofstream os{std::format("{}.{}", export_name, ext),
                       std::ios::binary}) {
    /**
        Header
        - Magic
        - Version (1 byte)
        - Number of entries (sizeof std::size_t)
     */
    os << magic;
    os << version;
    std::uint64_t num = entries.size();
    os.write(reinterpret_cast<const char *>(&num), sizeof(num));
    // Hidden xor-key for guessing
    std::clog << "Xor key:" << xor_key << '[' << xor_key_sz << "]\n";

    // Track absolute position for fast read
    uint64_t base_size = magic.size() + sizeof(version) + sizeof(num);

    /**
        Entries metadata
        - Zero-terminated filename
        - Custom metadata
        - File size
        - Data positions
     */
    // Precalculate the metadata positions
    for (auto &[name, e] : entries) {
      // Include zero byte at the end of the string
      base_size += name.size() + 1;
      // For extra metadata, filesize, and pos
      base_size += sizeof(uint64_t) + 2 * sizeof(std::size_t);
    }
    // Adding metadata to the archive
    for (auto &[name, e] : entries) {
      os << name << '\0';
      uint64_t metadata = 0;
      // Boolean
      uint8_t tf = 0;
      tf |= e.is_xor_enabled();
      tf <<= 1;
      tf |= e.get_compression().has_value();
      metadata |= static_cast<uint64_t>(tf);
      // Compression level
      metadata |=
          static_cast<uint64_t>(e.get_compression().value_or(0xFF) & 0xFF) << 8;
      auto size = e.get_size();
      os.write(reinterpret_cast<const char *>(&metadata), sizeof(metadata));
      os.write(reinterpret_cast<const char *>(&size), sizeof(size));
      os.write(reinterpret_cast<const char *>(&base_size), sizeof(base_size));
      base_size += size;
    }

    /**
        Entries data
     */
    for (auto &[_, e] : entries) {
      std::clog << "Adding to archive " << _ << '\n';
      os << e;
    }
    os.close();
  }
}
} // namespace Myr