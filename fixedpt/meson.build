project(
  'fp-perf-exp', 'C',
  version : '0.0.1',
  license: 'gpl-2.0',
  default_options: [
    'buildtype=debugoptimized',
    'c_std=gnu99',
    'warning_level=2'
  ],
  meson_version : '>= 0.38.0')

cc = meson.get_compiler('c')
host_system = host_machine.system()
add_project_arguments([ '-D_GNU_SOURCE' ], language: 'c')

# Version
_version = meson.project_version()
version_array = _version.split('.')
_major_version = version_array[0].to_int()
_minor_version = version_array[1].to_int()
_micro_version = version_array[2].to_int()

# Compiler and linker flags
common_cflags = []
common_ldflags = []

test_cflags = [
  '-ffast-math',
  '-fstrict-aliasing',
  '-Wpointer-arith',
  '-Wmissing-declarations',
  '-Wformat=2',
  '-Wstrict-prototypes',
  '-Wmissing-prototypes',
  '-Wnested-externs',
  '-Wold-style-definition',
  '-Wunused',
  '-Wuninitialized',
  '-Wshadow',
  '-Wmissing-noreturn',
  '-Wmissing-format-attribute',
  '-Wredundant-decls',
  '-Wlogical-op',
  '-Wcast-align',
  '-Wno-unused-local-typedefs',
  '-Werror=implicit',
  '-Werror=init-self',
  '-Werror=main',
  '-Werror=missing-braces',
  '-Werror=return-type',
  '-Werror=array-bounds',
  '-Werror=write-strings',
]

foreach cflag: test_cflags
  if cc.has_argument(cflag)
    common_cflags += cflag
  endif
endforeach

if host_system == 'linux'
  foreach ldflag: [ '-Wl,-Bsymbolic-functions', '-Wl,-z,relro', '-Wl,-z,now', ]
    if cc.has_argument(ldflag)
      common_ldflags += ldflag
    endif
  endforeach
endif

# Headers
conf = configuration_data()

conf.set('HAVE_STDLIB_H', cc.has_header('stdlib.h'),
         description: 'Define if stdlib.h is available')
conf.set('HAVE_STDINT_H', cc.has_header('stdint.h'),
         description: 'Define if stdint.h is available')
conf.set('HAVE_STDBOOL_H', cc.has_header('stdbool.h'),
         description: 'Define if stdbool.h is available')
conf.set('HAVE_MEMORY_H', cc.has_header('memory.h'),
         description: 'Define if memory.h is available')

# Debugging
debug_flags = []
buildtype = get_option('buildtype')
if buildtype == 'release'
  debug_flags += [ '-DG_DISABLE_ASSERT' ]
elif buildtype.startswith('debug')
  debug_flags += [ '-D_ENABLE_DEBUG' ]
endif

subdir('src')
