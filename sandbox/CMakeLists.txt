cmake_minimum_required(VERSION 3.8)

project(oui_sandbox) 

add_compile_definitions(
	OUI_TEST_APP
)

set(SRC ../oui/)
set(SUBSYSTEM WIN32)

if(WIN32)
	set(GLOBAL_OUT_DIR "C:/Users/Omid/Documents/cmake/out")
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${GLOBAL_OUT_DIR}/bin/${CMAKE_BUILD_TYPE}")
	set(LIBRARY_OUTPUT_PATH "${GLOBAL_OUT_DIR}/lib/${CMAKE_BUILD_TYPE}")
	
	link_directories("${GLOBAL_OUT_DIR}/lib/${CMAKE_BUILD_TYPE}")
	include_directories(${PROJECT_NAME} "${GLOBAL_OUT_DIR}/include/ocom" "../oui/")
endif()

add_executable(oui_sandbox ${SUBSYSTEM}
	main.cpp
	container.cpp
	${SRC}agg_arc.cpp
	${SRC}agg_bezier_arc.cpp
	${SRC}agg_bspline.cpp
	${SRC}agg_curves.cpp
	${SRC}agg_embedded_raster_fonts.cpp
	${SRC}agg_line_aa_basics.cpp
	${SRC}agg_line_profile_aa.cpp
	${SRC}agg_rounded_rect.cpp
	${SRC}agg_sqrt_tables.cpp
	${SRC}agg_svg_parser.cpp
	${SRC}agg_svg_path_renderer.cpp
	${SRC}agg_svg_path_tokenizer.cpp
	${SRC}agg_trans_affine.cpp
	${SRC}agg_vcgen_bspline.cpp
	${SRC}agg_vcgen_contour.cpp
	${SRC}agg_vcgen_dash.cpp
	${SRC}agg_vcgen_stroke.cpp
	${SRC}agg_vpgen_clip_polygon.cpp
	${SRC}agg_vpgen_clip_polyline.cpp
	${SRC}oui.cpp
	${SRC}oui_basics.cpp
	${SRC}oui_blur.cpp
	${SRC}oui_button.cpp
	${SRC}oui_canvas.cpp
	${SRC}oui_color.cpp
	${SRC}oui_label.cpp
	${SRC}oui_menu.cpp
	${SRC}oui_scroll.cpp
	${SRC}oui_sheet.cpp
	${SRC}oui_slot_cacher.cpp
	${SRC}oui_stack.cpp
	${SRC}oui_timer.cpp
	${SRC}oui_uix.cpp
	${SRC}oui_variable.cpp
	${SRC}oui_visual.cpp
	${SRC}oui_visual_basics.cpp
	${SRC}oui_window.cpp
	${SRC}oui_window_manager.cpp
	${SRC}oui_check.cpp
	${SRC}oui_text.cpp
	${SRC}oui_number.cpp
	${SRC}oui_editable_label.cpp
	${SRC}oui_text_search.cpp
	${SRC}oui_radio.cpp
	${SRC}oui_group.cpp
	${SRC}oui_link.cpp
)

target_link_libraries(oui_sandbox ocom)

set_property(TARGET oui_sandbox PROPERTY CXX_STANDARD 17)

target_compile_options(oui_sandbox PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W0>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-w>
)
