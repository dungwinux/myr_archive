// myr_archive.hpp : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <algorithm>
#include <filesystem>
#include <format>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <numeric>
#include <random>
#include <string>

namespace Myr {
constexpr auto ext = "myr";
/**
 *  TODO: Compile-time key
 */
constexpr const char xor_key[] = "\x8e\x65\x3c\x6d\x49\xb6\x55\x8b\x15\xf0\x80";
constexpr auto xor_key_sz = sizeof(xor_key) - 1;

class Entry {

private:
  std::filesystem::path path;
  std::string filename;
  std::size_t size;
  std::optional<uint8_t> compression{};
  bool xor_encrypt = true;

public:
  Entry(const std::filesystem::path &, std::size_t size);
  void set_filename(const std::string &);
  auto get_size() const;
  auto get_path() const;
  auto get_compression() const;
  auto is_xor_enabled() const;

  void set_compression(uint8_t);
  void rem_compression();
  const std::string &get_filename() const;
  friend std::ostream &operator<<(std::ostream &os, const Entry &e);
};

using MapKey = std::string;

class Archive {

private:
  std::map<MapKey, Entry> entries{};
  std::string magic{"MyrA"};
  const uint8_t version = 1;

public:
  Archive() = default;
  void add_entry(const Entry &);
  Myr::Entry &get_entry(const MapKey &);
  void remove_entry(const MapKey &);
  void try_export(const std::string &);
};
} // namespace Myr