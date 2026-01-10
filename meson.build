project(
  'helloworld',
  'c',
  meson_version : '>= 1.3.0',
  version : '0.0.1',
  default_options : ['warning_level=3'],
)

dependencies = [
]

include_dirs = include_directories('.')

sources = [
  'src/main.c',
  'src/global_error.c',
  'src/tokenize.c'
]

exe = executable(
  'helloworld',
  sources,
  dependencies : dependencies,
  include_directories: include_dirs,
  install : true,
)

test('basic', exe)
