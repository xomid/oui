cmake_minimum_required (VERSION 3.8)

add_compile_definitions(
	_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
	DEBUG_UPDATEFRAME
)

find_package(Freetype CONFIG REQUIRED)
find_package(harfbuzz CONFIG REQUIRED)
find_package(expat CONFIG REQUIRED)
include_directories(${FREETYPE_INCLUDE_DIRS})
include_directories(${HARFBUZZ_INCLUDE_DIRS})

set(HARFBUZZ_LIBRARY_RELEASE C:/dev/vcpkg/installed/x64-windows/bin)
set(LIBTYPE SHARED)
set(LIBNAME oui)

add_library(${LIBNAME} ${LIBTYPE} 
	agg_arc.cpp
	agg_bezier_arc.cpp
	agg_bspline.cpp
	agg_curves.cpp
	agg_embedded_raster_fonts.cpp
	agg_line_aa_basics.cpp
	agg_line_profile_aa.cpp
	agg_rounded_rect.cpp
	agg_sqrt_tables.cpp
	agg_svg_parser.cpp
	agg_svg_path_renderer.cpp
	agg_svg_path_tokenizer.cpp
	agg_trans_affine.cpp
	agg_vcgen_bspline.cpp
	agg_vcgen_contour.cpp
	agg_vcgen_dash.cpp
	agg_vcgen_stroke.cpp
	agg_vpgen_clip_polygon.cpp
	agg_vpgen_clip_polyline.cpp
	oui.cpp
	oui_basics.cpp
	oui_blur.cpp
	oui_button.cpp
	oui_canvas.cpp
	oui_color.cpp
	oui_label.cpp
	oui_menu.cpp
	oui_scroll.cpp
	oui_sheet.cpp
	oui_slot_cacher.cpp
	oui_stack.cpp
	oui_timer.cpp
	oui_uix.cpp
	oui_variable.cpp
	oui_visual.cpp
	oui_visual_basics.cpp
	oui_window.cpp
	oui_window_manager.cpp
	#oui_check.cpp
	#oui_console.cpp
	#oui_dumpster.cpp
	#oui_filter_table.cpp
	#oui_flexible_text_list.cpp
	#oui_grid.cpp
	#oui_group.cpp
	#oui_link.cpp
	#oui_m_auto_complete.cpp
	#oui_option.cpp
	#oui_radio.cpp
	#oui_row.cpp
	#oui_select.cpp
	#oui_table.cpp
	#oui_text.cpp
	#oui_text_search.cpp
)

target_link_libraries(${LIBNAME} freetype harfbuzz expat::expat)

set_property(TARGET ${LIBNAME} PROPERTY CXX_STANDARD 17)