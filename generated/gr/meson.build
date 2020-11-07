generate_version = files(meson.source_root() / 'generate-version')
version = custom_target('version',
  output: 'version.h',
  build_always_stale: true,
  command: [generate_version, meson.source_root(), meson.project_version(), get_option('version-tag'), '@OUTPUT@']
  )
