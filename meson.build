project('gild-root', 'c')

add_global_arguments(
  '-Wall', '-Wextra',
  '-Wundef', '-Wold-style-definition', '-Wmissing-prototypes', '-Wstrict-prototypes',
  '-fdiagnostics-color=always',
  '-Werror',
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

executable('gild-root', 'gild-root.c')
t_cmdline = executable('test-cmdline', 'test-cmdline.c', 'cmdline.c', native: true)
t_grabfile = executable('test-grabfile', 'test-grab-file.c', 'grab-file.c', native: true)

test_files = files(['grab-file.c'])
test('cmdline', t_cmdline)
test('grabfile', t_grabfile, args: test_files)
