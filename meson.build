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

executable('gild-root',
  'gild-root.c', 'grab-file.c', 'cmdline.c', version,
  install: true,
  install_dir: get_option('rootlibexecdir') / 'gild-root'
  )

t_cmdline = executable('test-cmdline', 'test-cmdline.c', 'cmdline.c', native: true)
t_grabfile = executable('test-grabfile', 'test-grab-file.c', 'grab-file.c', native: true)

test_files = files(['grab-file.c'])
test('cmdline', t_cmdline)
test('grabfile', t_grabfile, args: test_files)
