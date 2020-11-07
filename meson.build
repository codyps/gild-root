project('gild-root', 'c',
  version: '0.0.0',
  default_options: [
    'c_std=gnu18',
    'b_lto=true',
    'b_sanitizers=address,undefined',
    'warn_level=2',
    ],
  license: 'AGPL-v3-or-later',
  meson_version: '>=0.50',
  )

conf = configuration_data()
conf.set('PROJECT_VERSION', meson.project_version())

cg_flags = []

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

custom_target('version',
  output: 'gr-version.h',
  command: ['./generate-version', meson.current_source_dir(), meson.project_version(), get_option('version-tag'), '@OUTPUT@']
  )

executable('gild-root',
  'gild-root.c', 'grab-file.c', 'cmdline.c',
  install: true
  install_dir: get_option('rootlibexecdir') / 'gild-root'
  )

t_cmdline = executable('test-cmdline', 'test-cmdline.c', 'cmdline.c', native: true)
t_grabfile = executable('test-grabfile', 'test-grab-file.c', 'grab-file.c', native: true)

test_files = files(['grab-file.c'])
test('cmdline', t_cmdline)
test('grabfile', t_grabfile, args: test_files)
