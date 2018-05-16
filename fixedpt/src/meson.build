headers = [
]

private_headers = [
  'fixedpt/fixedptc.h',
  'utils.h'
]

deps = [
]

# Internal configuration header
configure_file(output: 'config.h',
               configuration: conf)

sources = files([
  'main.c'
])

executable('fp-perf-exp', sources,
           dependencies : deps,
           install : true)
