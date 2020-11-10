
t_cmdline = executable('test-cmdline', 'test-cmdline.c',
  dependencies: cmdline)
t_grabfile = executable('test-grabfile', 'test-grab-file.c',
  dependencies: grab_file)
t_parsemountflags = executable('test-parse-mount-flags', 'test-parse-mount-flags.c',
  dependencies: parse_mount_flags)

test_files = files(['test-grab-file.c'])
test('cmdline', t_cmdline)
test('grabfile', t_grabfile, args: test_files)
test('parse-mount-flags', t_parsemountflags)
