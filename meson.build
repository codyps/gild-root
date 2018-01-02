project('gild-root', 'c')

#git = find_program('git')
custom_target('version',
    output: 'generated/gr/version.h'
    command: ['./generate-version'])

executable('gild-root', 'gild-root.c')
