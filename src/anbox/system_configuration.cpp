/*
 * Copyright (C) 2016 Simon Fels <morphis@gravedo.de>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#include "anbox/system_configuration.h"
#include "anbox/utils.h"
#include "anbox/build/config.h"

#include "external/xdg/xdg.h"

#include <cstring>

namespace fs = boost::filesystem;

namespace {
static std::string runtime_dir() {
  static std::string path;
  if (path.empty()) {
    path = anbox::utils::get_env_value("XDG_RUNTIME_DIR", "");
    if (path.empty())
      BOOST_THROW_EXCEPTION(std::runtime_error("No runtime directory specified"));
  }
  return path;
}
}

void anbox::SystemConfiguration::set_container_id(const std::string &id, bool nothrow) {
  static std::string dir = anbox::utils::string_format("%s/anbox/%s", runtime_dir(), id);
  if (fs::is_directory(fs::path(dir)) && !nothrow)
    BOOST_THROW_EXCEPTION(std::runtime_error("Container name provided already in use"));
  container_id = id;
}

void anbox::SystemConfiguration::set_data_path(const std::string &path) {
  data_path = path;
}

fs::path anbox::SystemConfiguration::data_dir() const {
  return data_path;
}

std::string anbox::SystemConfiguration::container_name() const {
  return container_id;
}

std::string anbox::SystemConfiguration::rootfs_dir() const {
  return (data_path / "rootfs").string();
}

std::string anbox::SystemConfiguration::log_dir() const {
  return (data_path / container_id / "logs").string();
}

std::string anbox::SystemConfiguration::container_config_dir() const {
  return (data_path / container_id / "containers").string();
}

std::string anbox::SystemConfiguration::container_socket_path() const {
  return "/run/anbox-container.socket";
}

std::string anbox::SystemConfiguration::instance_dir() const {
  static std::string dir = anbox::utils::string_format("%s/anbox/%s",
						       runtime_dir(), container_id);
  return dir;
}

std::string anbox::SystemConfiguration::socket_dir() const {
  static std::string dir = anbox::utils::string_format("%s/anbox/%s/sockets",
						       runtime_dir(), container_id);
  return dir;
}

std::string anbox::SystemConfiguration::input_device_dir() const {
  static std::string dir = anbox::utils::string_format("%s/anbox/%s/input",
						       runtime_dir(), container_id);
  return dir;
}

std::string anbox::SystemConfiguration::application_item_dir() const {
  static auto dir = xdg::data().home() / "applications" / "anbox" / container_id;
  return dir.string();
}

std::string anbox::SystemConfiguration::resource_dir() const {
  return resource_path.string();
}

anbox::SystemConfiguration& anbox::SystemConfiguration::instance() {
  static SystemConfiguration config;
  return config;
}

anbox::SystemConfiguration::SystemConfiguration() {
  auto detect_resource_path = [] () -> fs::path {
    const auto snap_path = utils::get_env_value("SNAP");
    if (!snap_path.empty()) {
      return fs::path(snap_path) / "usr" / "share" / "anbox";
    } else {
      return anbox::build::default_resource_path;
    }
  };

  resource_path = detect_resource_path();
  data_path = anbox::build::default_data_path;
}
