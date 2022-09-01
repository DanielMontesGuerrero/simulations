load('//:subdir_glob.bzl', 'subdir_glob')

cxx_binary(
    name='gameoflife',
    header_namespace='gameoflife',
    headers=subdir_glob([
        ('gameoflife/include', '**/*.hpp'),
    ]),
    srcs=glob([
        'gameoflife/src/**/*.cpp',
    ]),
    deps=[
        '//utilscpp:utilscpp',
    ],
    linker_flags=[
        '-LSDL2main',
        '-lSDL2',
    ],
)
