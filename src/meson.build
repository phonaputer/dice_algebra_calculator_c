project(
  'helloworld',
  'c',
  meson_version : '>= 1.3.0',
  version : '0.0.1',
  default_options : ['warning_level=3'],
)

dependencies = [
]

sources = [
  'hello.c',
]

exe = executable(
  'helloworld',
  sources,
  dependencies : dependencies,
  install : true,
)

test('basic', exe)
