project(
    '42sh',
    'c',
    version: 'dev',
    default_options: [
        'c_std=c99',
    ],
)

cflags = ['-Werror', '-Wall', '-Wextra', '-pedantic']
add_project_arguments(cflags, language: 'c')


sources = []
subdir('src')

test = include_directories('src')
bin = executable(
    '42sh',
    sources,
    include_directories: test,
    install: true,
)