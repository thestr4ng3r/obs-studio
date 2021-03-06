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

#ifdef __cplusplus
extern "C" {
#endif

struct gs_vertex_buffer {
	struct gs_vb_data *data;
};

enum cpu_shader_kind {
	CPU_SHADER_UNKNOWN,
	CPU_SHADER_DEFAULT_DRAW_VERTEX,
	CPU_SHADER_DEFAULT_DRAW_FRAGMENT,
	CPU_SHADER_DEFAULT_DRAW_ALPHA_DIVIDE_VERTEX,
	CPU_SHADER_DEFAULT_DRAW_ALPHA_DIVIDE_FRAGMENT,
	CPU_SHADER_OPAQUE_VERTEX,
	CPU_SHADER_OPAQUE_FRAGMENT,
	CPU_SHADER_SOLID_VERTEX,
	CPU_SHADER_SOLID_FRAGMENT,
	CPU_SHADER_SOLID_COLORED_VERTEX,
	CPU_SHADER_SOLID_COLORED_FRAGMENT,
	CPU_SHADER_SOLID_RANDOM_VERTEX,
	CPU_SHADER_SOLID_RANDOM_FRAGMENT,
	CPU_SHADER_FORMAT_CONVERSION_NV12_Y_VERTEX,
	CPU_SHADER_FORMAT_CONVERSION_NV12_Y_FRAGMENT,
	CPU_SHADER_FORMAT_CONVERSION_NV12_UV_VERTEX,
	CPU_SHADER_FORMAT_CONVERSION_NV12_UV_FRAGMENT,
};

struct gs_shader {
	struct gs_device *device;
	enum cpu_shader_kind kind;
	char *file;
};

enum cpu_shader_param_kind {
	CPU_SHADER_PARAM_UNKNOWN,
	CPU_SHADER_PARAM_IMAGE,
	CPU_SHADER_PARAM_COLOR_VEC0,
	CPU_SHADER_PARAM_COLOR_VEC1,
	CPU_SHADER_PARAM_COLOR_VEC2
};

struct gs_shader_param {
	struct gs_device *device;
	struct gs_shader *shader;
	char *name;
	enum cpu_shader_param_kind kind;
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
	gs_texture_t *tex;
};

struct gs_swap_chain {
	gs_device_t *device;
	struct gs_init_data info;
	struct cpu_windowinfo *wi;
};

struct gs_device {
	uint32_t width;
	uint32_t height;
	struct cpu_platform *plat;
	struct {
		enum gs_blend_type src_c;
		enum gs_blend_type dest_c;
		enum gs_blend_type src_a;
		enum gs_blend_type dest_a;
	} blend;
	bool blending_enabled;
	bool depth_test_enabled;
	enum gs_cull_mode cull_mode;
	struct matrix4 cur_proj;
	DARRAY(struct matrix4) proj_stack;
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
	gs_swapchain_t *swapchain_cur;
	struct {
		gs_texture_t *image;

		// format conversion
		struct vec4 color_vec0;
		struct vec4 color_vec1;
		struct vec4 color_vec2;
	} params;
};

struct cpu_blit_params {
	gs_texture_t *src;
	gs_texture_t *dst;
	int64_t src_x, src_y, src_width, src_height;
	int64_t dst_x, dst_y, dst_width, dst_height;
};

#define CPU_BLIT_PARAMS_UNPACK \
	gs_texture_t *src = params.src; \
	gs_texture_t *dst = params.dst; \
	int64_t src_x = params.src_x; \
	int64_t src_y = params.src_y; \
	int64_t src_width = params.src_width; \
	int64_t src_height = params.src_height; \
	int64_t dst_x = params.dst_x; \
	int64_t dst_y = params.dst_y; \
	int64_t dst_width = params.dst_width; \
	int64_t dst_height = params.dst_height;

size_t cpu_tex_data_size(gs_texture_t *tex);
void cpu_blit_texture(struct cpu_blit_params params);
bool cpu_platform_init_swapchain(struct gs_swap_chain *swap);
void cpu_platform_fini_swapchain(struct gs_swap_chain *swap);
void cpu_platform_resize_swapchain(struct gs_swap_chain *swap, uint32_t width, uint32_t height);
struct cpu_platform *cpu_platform_create(gs_device_t *device, uint32_t adapter);
void cpu_platform_destroy(struct cpu_platform *plat);
void cpu_platform_blit(struct gs_device *device, struct cpu_blit_params params);
void cpu_rgba_to_nv12_y(gs_texture_t *src, gs_texture_t *dst, struct vec4 color_vec0);

#ifdef __cplusplus
}
#endif