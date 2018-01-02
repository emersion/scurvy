project(
	'scurvy',
	'c',
	license: 'MIT',
	meson_version: '>=0.43.0',
	default_options: [
		'c_std=c11',
		'warning_level=2',
		'werror=true',
	],
)

add_project_arguments('-Wno-unused-parameter', language: 'c')

scurvy_inc = include_directories('include')

wayland_client = dependency('wayland-client')
wayland_protos = dependency('wayland-protocols')
wayland_cursor = dependency('wayland-cursor')
vterm = dependency('vterm')
cairo = dependency('cairo')
pango = dependency('pango')
pangocairo = dependency('pangocairo')
xkbcommon = dependency('xkbcommon')

subdir('protocol')

executable(
	'scurvy',
	files(
		'src/main.c',
		'src/wayland/buffers.c',
		'src/wayland/main.c',
		'src/wayland/registry.c',
		'src/wayland/window.c',
		'src/ini.c',
		'src/colors.c',
		'src/config.c',
		'src/log.c',
		'src/pango.c',
		'src/child.c',
		'src/term.c',
		'src/utf8.c',
	),
	include_directories: scurvy_inc,
	dependencies: [
		wayland_cursor,
		vterm,
		cairo,
		pango,
		pangocairo,
		xkbcommon,
		scurvy_protos,
	],
)
