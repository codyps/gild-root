project('gild-root', 'c',
  version: '0.0.0',
  default_options: [
    'c_std=gnu18',
    'b_lto=true',
    'b_sanitize=address,undefined',
    'warning_level=2',
    ],
  license: 'AGPL-v3-or-later',
  meson_version: '>=0.50',
  )

cg_flags = []

project_source_root = meson.current_source_dir()

add_global_arguments(
  cg_flags,
  language: 'c'
  )

add_project_arguments(
  '-Wundef', '-Wold-style-definition', '-Wmissing-prototypes', '-Wstrict-prototypes',
  '-Wwrite-strings',
  language: 'c'
  )

add_global_link_arguments(
  language: 'c'
  )

subdir('generated/gr')

cmdline_sta = static_library('cmdline',
  'cmdline.c')

cmdline = declare_dependency(
  link_with: cmdline_sta,
  include_directories: '.',
  )

grab_file_sta = static_library('grab_file',
  'grab-file.c')

grab_file = declare_dependency(
  link_with: grab_file_sta,
  include_directories: '.',
  )

executable('gild-root',
  'gild-root.c', version,
  dependencies: [grab_file, cmdline],
  install: true,
  install_dir: get_option('rootlibexecdir') / 'gild-root'
  )

subdir('tests')
