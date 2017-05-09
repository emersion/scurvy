include(Wayland)
find_package(Wayland REQUIRED)
find_package(XKBCommon REQUIRED)

WAYLAND_ADD_PROTOCOL_CLIENT(proto-client-xdg-shell
	xdg-shell.xml
	xdg-shell
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    ${CMAKE_BINARY_DIR}/wayland/
	${XKBCOMMON_INCLUDE_DIRS}
	${WAYLAND_INCLUDE_DIR}
    ${PANGO_INCLUDE_DIRS}
)

add_library(scurvy-wayland STATIC
	${proto-client-xdg-shell}
    main.c
    registry.c
    window.c
    buffers.c
)

target_link_libraries(scurvy-wayland
	${PANGO_LIBRARIES}
	${XKBCOMMON_LIBRARIES}
	${EPOLLSHIM_LIBRARIES}
	${WAYLAND_CLIENT_LIBRARIES}
	${WAYLAND_CURSOR_LIBRARIES}
	${CAIRO_LIBRARIES}
	${PANGO_LIBRARIES}
)
