project('ptraccel', 'c', 'cpp',
	version : '1',
	license : 'MIT/Expat',
	default_options : [ 'c_std=gnu99', 'warning_level=2' ],
	meson_version : '>= 0.40.0')

cc = meson.get_compiler('c')
cppflags = ['-Wno-unused-parameter', '-g', '-fvisibility=hidden']
cflags = cppflags + ['-Wmissing-prototypes', '-Wstrict-prototypes']
add_project_arguments(cflags, language : 'c')
add_project_arguments(cppflags, language : 'cpp')
defines = ['-DPTRACCEL_DEBUGGING']
add_project_arguments(defines, language : 'c')

dep_lm = cc.find_library('m')

sources = [ 'ptrveloc.c',
            'ptrveloc.h',
            'input.h',
            'valuator-mask.h',
            'valuator-mask.c' ]
lib = static_library('ptraccel',
                     sources,
                     dependencies : [dep_lm],
                     include_directories : [])
dep_lib = declare_dependency(link_with: lib)
