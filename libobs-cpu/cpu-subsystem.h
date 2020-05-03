/******************************************************************************
    Copyright (C) 2020 by thestr4ng3r

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#pragma once

#include <util/darray.h>
#include <util/threading.h>
#include <graphics/graphics.h>
#include <graphics/device-exports.h>
#include <graphics/matrix4.h>

struct gs_vertex_buffer {
	struct gs_vb_data *data;
};

struct gs_shader {

};

struct gs_shader_param {

};

struct gs_sampler_state {

};

struct gs_texture {
	enum gs_texture_type type;
	uint32_t width;
	uint32_t height;
	uint32_t levels;
	enum gs_color_format color_format;

	uint8_t *data; // size = (gs_get_format_bpp(color_format) / 8) * width * height * levels;
};

struct gs_stage_surface {

};

struct gs_device {
	struct {
		enum gs_blend_type src_c;
		enum gs_blend_type dest_c;
		enum gs_blend_type src_a;
		enum gs_blend_type dest_a;
	} blend;
	bool blending_enabled;
	bool depth_test_enabled;
	enum gs_cull_mode cull_mode;
	struct matrix4 cur_vp;
	struct matrix4 cur_proj;
	struct {
		gs_texture_t *tex;
		gs_zstencil_t *zstencil;
	} render_target;
	struct {
		int x;
		int y;
		int width;
		int height;
	} viewport;
	gs_shader_t *vertex_shader_cur;
	gs_shader_t *fragment_shader_cur;
	gs_vertbuffer_t *vertex_buffer_cur;
	gs_indexbuffer_t *index_buffer_cur;
};
