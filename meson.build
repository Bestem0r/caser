project(
  'caser', 'c',
  version : '0.1.0',
  license : 'GPL-3.0',
  default_options : [
    'warning_level=2',
    'c_std=gnu11',
  ])

gnome = import('gnome')
libadwaita_dep = dependency('libadwaita-1')

executable(
  'caser',
  'src/main.c',
  dependencies : [libadwaita_dep],
  install : true,
)