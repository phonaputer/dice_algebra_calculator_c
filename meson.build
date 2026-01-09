project(
  'helloworld',
  'c',
  meson_version : '>= 1.3.0',
  version : '0.0.1',
  default_options : ['warning_level=3'],
)

dependencies = [
]

include_dirs = include_directories('include')

sources = [
  'src/main.c',
  'src/global_error.c'
]

exe = executable(
  'helloworld',
  sources,
  include_directories: include_dirs,
  dependencies : dependencies,
  install : true,
)

test('basic', exe)
