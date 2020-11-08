generate_version = files(project_source_root / 'generate-version')
version = custom_target('version',
  output: 'version.h',
  build_always_stale: true,
  command: [generate_version, project_source_root, meson.project_version(), get_option('version-tag'), '@OUTPUT@']
  )
