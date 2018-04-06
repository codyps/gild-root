project('gild-root', 'c')

cg_flags = [
  '-fsanitize=undefined',
  '-fsanitize=address',
  '-ggdb3'
]

if meson.get_compiler('c').get_id() == 'gcc'
  cg_flags += '-fvar-tracking-assignments'
endif

add_global_arguments(
  '-Wall', '-Wextra',
  '-Wundef', '-Wold-style-definition', '-Wmissing-prototypes', '-Wstrict-prototypes',
  '-Wwrite-strings',
  '-fdiagnostics-color=always',
  '-Werror',
  cg_flags,
  language: 'c'
)

add_global_link_arguments(
  cg_flags,
  language: 'c'
)

# TODO: options
#  - sanitize
#  - lto
#  - diag color?
#  - werror

#git = find_program('git')
custom_target('version',
    output: 'gr-version.h',
    command: ['./generate-version', '@OUTPUT@']
)

executable('gild-root', 'gild-root.c', 'grab-file.c', 'cmdline.c')
t_cmdline = executable('test-cmdline', 'test-cmdline.c', 'cmdline.c', native: true)
t_grabfile = executable('test-grabfile', 'test-grab-file.c', 'grab-file.c', native: true)

test_files = files(['grab-file.c'])
test('cmdline', t_cmdline)
test('grabfile', t_grabfile, args: test_files)
