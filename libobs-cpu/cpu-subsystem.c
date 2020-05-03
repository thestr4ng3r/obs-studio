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

#include <graphics/matrix3.h>
#include <graphics/vec2.h>
#include <graphics/graphics.h>
#include "cpu-subsystem.h"

const char *device_get_name(void)
{
	return "CPU";
}

int device_get_type(void)
{
	return GS_DEVICE_CPU;
}

const char *device_preprocessor_name(void)
{
	return "_CPU";
}

int device_create(gs_device_t **p_device, uint32_t adapter)
{
	struct gs_device *device = bzalloc(sizeof(struct gs_device));
	int errorcode = GS_ERROR_FAIL;

	blog(LOG_INFO, "---------------------------------");
	blog(LOG_INFO, "Initializing CPU Renderer...");

	blog(LOG_INFO, "CPU Renderer loaded.");

	*p_device = device;
	return GS_SUCCESS;
}

void device_destroy(gs_device_t *device)
{
	if (!device) {
		return;
	}
	bfree(device);
}

static void log_unimplemented(const char *fcn_name)
{
	blog(LOG_ERROR, "%s is unimplemented.", fcn_name);
}

#define UNIMPLEMENTED log_unimplemented(__PRETTY_FUNCTION__);
#define UNIMPLEMENTED_RET(r) UNIMPLEMENTED return r;

void device_enter_context(gs_device_t *device)
{
}

void device_leave_context(gs_device_t *device)
{
}

gs_vertbuffer_t *device_vertexbuffer_create(gs_device_t *device, struct gs_vb_data *data, uint32_t flags)
{
	gs_vertbuffer_t *r = bzalloc (sizeof (gs_vertbuffer_t));
	r->data = data;
	return r;
}

void gs_vertexbuffer_destroy(gs_vertbuffer_t *vb)
{
	if(!vb)
		return;
	gs_vbdata_destroy(vb->data);
	bfree(vb);
}

struct gs_vb_data *gs_vertexbuffer_get_data(const gs_vertbuffer_t *vb)
{
	return vb->data;
}

void gs_vertexbuffer_flush(gs_vertbuffer_t *vertbuffer)
{
}

void device_load_vertexbuffer(gs_device_t *device, gs_vertbuffer_t *vb)
{
	device->vertex_buffer_cur = vb;
}

void device_load_indexbuffer(gs_device_t *device, gs_indexbuffer_t *ib)
{
	device->index_buffer_cur = ib;
}

gs_shader_t *device_vertexshader_create(gs_device_t *device, const char *shader, const char *file, char **error_string)
{
	return bzalloc(sizeof(gs_shader_t));
}

gs_shader_t *device_pixelshader_create(gs_device_t *device, const char *shader, const char *file, char **error_string)
{
	return bzalloc(sizeof(gs_shader_t));
}

void device_load_vertexshader(gs_device_t *device, gs_shader_t *vertshader)
{
	device->vertex_shader_cur = vertshader;
}

void device_load_pixelshader(gs_device_t *device, gs_shader_t *pixelshader)
{
	device->fragment_shader_cur = pixelshader;
}

void gs_shader_destroy(gs_shader_t *shader)
{
	bfree(shader);
}

gs_sparam_t *gs_shader_get_param_by_name(gs_shader_t *shader, const char *name)
{
	return bzalloc(sizeof(gs_sparam_t));
}

void gs_shader_get_param_info(const gs_sparam_t *param, struct gs_shader_param_info *info)
{
	// TODO
	info->type = GS_SHADER_PARAM_UNKNOWN;
	info->name = "unknown";
}

gs_samplerstate_t *device_samplerstate_create(gs_device_t *device, const struct gs_sampler_info *info)
{
	return bzalloc(sizeof(gs_samplerstate_t));
}

void gs_samplerstate_destroy(gs_samplerstate_t *samplerstate)
{
	bfree(samplerstate);
}

static size_t tex_data_size(gs_texture_t *tex)
{
	size_t bpp = gs_get_format_bpp(tex->color_format) / 8;
	return bpp * tex->width * tex->height * tex->levels;
}

gs_texture_t *device_texture_create(gs_device_t *device, uint32_t width, uint32_t height, enum gs_color_format color_format, uint32_t levels, const uint8_t **data, uint32_t flags)
{
	gs_texture_t *r = bzalloc(sizeof(gs_texture_t));
	r->type = GS_TEXTURE_2D;
	r->width = width;
	r->height = height;
	r->color_format = color_format;
	r->levels = levels;
	size_t size = tex_data_size(r);
	r->data = bmalloc(size);
	if(data && *data)
		memcpy(r->data, *data, size);
	return r;
}

enum gs_texture_type device_get_texture_type(const gs_texture_t *texture)
{
	return texture->type;
}

void gs_texture_destroy(gs_texture_t *tex)
{
	if(!tex)
		return;
	bfree(tex->data),
	bfree(tex);
}

bool gs_texture_is_rect(const gs_texture_t *tex)
{
	// TODO
	return true;
}

uint32_t gs_texture_get_width(const gs_texture_t *tex)
{
	return tex->width;
}

uint32_t gs_texture_get_height(const gs_texture_t *tex)
{
	return tex->height;
}

gs_stagesurf_t *device_stagesurface_create(gs_device_t *device, uint32_t width, uint32_t height, enum gs_color_format color_format)
{
	return bzalloc(sizeof(gs_stagesurf_t));
}

void gs_stagesurface_destroy(gs_stagesurf_t *stagesurf)
{
	bfree(stagesurf);
}

void device_begin_frame(gs_device_t *device)
{
}

void device_begin_scene(gs_device_t *device)
{
}

void device_end_scene(gs_device_t *device)
{
}

void device_flush(gs_device_t *device)
{
}

void device_enable_blending(gs_device_t *device, bool enable)
{
	device->blending_enabled = enable;
}

void device_blend_function_separate(gs_device_t *device, enum gs_blend_type src_c, enum gs_blend_type dest_c, enum gs_blend_type src_a, enum gs_blend_type dest_a)
{
	device->blend.src_c = src_c;
	device->blend.dest_c = dest_c;
	device->blend.src_a = src_a;
	device->blend.dest_a = dest_a;
}

void device_enable_depth_test(gs_device_t *device, bool enable)
{
	device->depth_test_enabled = enable;
}

void device_set_cull_mode(gs_device_t *device, enum gs_cull_mode mode)
{
	device->cull_mode = mode;
}

void device_set_render_target(gs_device_t *device, gs_texture_t *tex, gs_zstencil_t *zstencil)
{
	device->render_target.tex = tex;
	device->render_target.zstencil = zstencil;
}

void device_clear(gs_device_t *device, uint32_t clear_flags, const struct vec4 *color, float depth, uint8_t stencil)
{
	if((clear_flags & GS_CLEAR_COLOR) && device->render_target.tex)
	{
		// TODO
	}

	if((clear_flags & GS_CLEAR_DEPTH) && device->render_target.zstencil)
	{
		// TODO
	}
}

void device_ortho(gs_device_t *device, float left, float right, float top, float bottom, float near, float far)
{
	struct matrix4 *dst = &device->cur_proj;

	float rml = right - left;
	float bmt = bottom - top;
	float fmn = far - near;

	vec4_zero(&dst->x);
	vec4_zero(&dst->y);
	vec4_zero(&dst->z);
	vec4_zero(&dst->t);

	dst->x.x = 2.0f / rml;
	dst->t.x = (left + right) / -rml;

	dst->y.y = 2.0f / -bmt;
	dst->t.y = (bottom + top) / bmt;

	dst->z.z = -2.0f / fmn;
	dst->t.z = (far + near) / -fmn;

	dst->t.w = 1.0f;
}

void device_set_viewport(gs_device_t *device, int x, int y, int width, int height)
{
	device->viewport.x = x;
	device->viewport.y = y;
	device->viewport.width = width;
	device->viewport.height = height;
}

void device_draw(gs_device_t *device, enum gs_draw_mode draw_mode, uint32_t start_vert, uint32_t num_verts)
{
	// TODO: Do the heavy stuff here
	blog(LOG_ERROR, "CPU Drawing unimplemented.");
}

void *device_get_device_obj(gs_device_t *device) { UNIMPLEMENTED_RET(NULL) }
gs_swapchain_t *device_swapchain_create(gs_device_t *device, const struct gs_init_data *info) {	UNIMPLEMENTED_RET(NULL) }
void device_resize(gs_device_t *device, uint32_t cx, uint32_t cy) { UNIMPLEMENTED }
void device_get_size(const gs_device_t *device, uint32_t *cx, uint32_t *cy) { UNIMPLEMENTED }
uint32_t device_get_width(const gs_device_t *device) { UNIMPLEMENTED_RET(0) }
uint32_t device_get_height(const gs_device_t *device) { UNIMPLEMENTED_RET(0) }
gs_timer_t *device_timer_create(gs_device_t *device) { UNIMPLEMENTED_RET(NULL) }
gs_timer_range_t *device_timer_range_create(gs_device_t *device) { UNIMPLEMENTED_RET(NULL) }
void device_load_texture(gs_device_t *device, gs_texture_t *tex, int unit) { UNIMPLEMENTED }
void device_load_samplerstate(gs_device_t *device, gs_samplerstate_t *ss, int unit) { UNIMPLEMENTED }
void device_load_default_samplerstate(gs_device_t *device, bool b_3d, int unit) { UNIMPLEMENTED }
gs_shader_t *device_get_vertex_shader(const gs_device_t *device) { UNIMPLEMENTED_RET(NULL) }
gs_shader_t *device_get_pixel_shader(const gs_device_t *device) { UNIMPLEMENTED_RET(NULL) }
gs_texture_t *device_get_render_target(const gs_device_t *device) { UNIMPLEMENTED_RET(NULL) }
gs_zstencil_t *device_get_zstencil_target(const gs_device_t *device) { UNIMPLEMENTED_RET(NULL) }
static bool get_tex_dimensions(gs_texture_t *tex, uint32_t *width, uint32_t *height) { UNIMPLEMENTED_RET(false) }
void device_set_cube_render_target(gs_device_t *device, gs_texture_t *cubetex, int side, gs_zstencil_t *zstencil) { UNIMPLEMENTED }
void device_copy_texture_region(gs_device_t *device, gs_texture_t *dst, uint32_t dst_x, uint32_t dst_y, gs_texture_t *src, uint32_t src_x, uint32_t src_y, uint32_t src_w, uint32_t src_h) { UNIMPLEMENTED }
void device_copy_texture(gs_device_t *device, gs_texture_t *dst, gs_texture_t *src) { UNIMPLEMENTED }
enum gs_cull_mode device_get_cull_mode(const gs_device_t *device) { UNIMPLEMENTED_RET(GS_BACK) }
void device_enable_stencil_test(gs_device_t *device, bool enable) { UNIMPLEMENTED }
void device_enable_stencil_write(gs_device_t *device, bool enable) { UNIMPLEMENTED }
void device_enable_color(gs_device_t *device, bool red, bool green, bool blue, bool alpha) { UNIMPLEMENTED }
void device_blend_function(gs_device_t *device, enum gs_blend_type src, enum gs_blend_type dest) { UNIMPLEMENTED }
void device_depth_function(gs_device_t *device, enum gs_depth_test test) { UNIMPLEMENTED }
void device_stencil_function(gs_device_t *device, enum gs_stencil_side side, enum gs_depth_test test) { UNIMPLEMENTED }
void device_stencil_op(gs_device_t *device, enum gs_stencil_side side, enum gs_stencil_op_type fail, enum gs_stencil_op_type zfail, enum gs_stencil_op_type zpass) { UNIMPLEMENTED }
void device_get_viewport(const gs_device_t *device, struct gs_rect *rect) { UNIMPLEMENTED }
void device_set_scissor_rect(gs_device_t *device, const struct gs_rect *rect) { UNIMPLEMENTED }
void device_frustum(gs_device_t *device, float left, float right, float top, float bottom, float near, float far) { UNIMPLEMENTED }
void device_projection_push(gs_device_t *device) { UNIMPLEMENTED }
void device_projection_pop(gs_device_t *device) { UNIMPLEMENTED }
void device_debug_marker_begin(gs_device_t *device, const char *markername, const float color[4]) { UNIMPLEMENTED }
void device_debug_marker_end(gs_device_t *device) {	UNIMPLEMENTED }

gs_texture_t *device_cubetexture_create(gs_device_t *device, uint32_t size, enum gs_color_format color_format,
		uint32_t levels, const uint8_t **data, uint32_t flags) { UNIMPLEMENTED_RET(NULL) }
gs_texture_t *device_voltexture_create(gs_device_t *device, uint32_t width, uint32_t height, uint32_t depth,
		enum gs_color_format color_format, uint32_t levels, const uint8_t *const *data, uint32_t flags) { UNIMPLEMENTED_RET(NULL) }
gs_zstencil_t *device_zstencil_create(gs_device_t *device, uint32_t width, uint32_t height, enum gs_zstencil_format format) { UNIMPLEMENTED_RET(NULL) }
gs_indexbuffer_t *device_indexbuffer_create(gs_device_t *device, enum gs_index_type type, void *indices, size_t num, uint32_t flags) { UNIMPLEMENTED_RET(NULL) }
void device_stage_texture(gs_device_t *device, gs_stagesurf_t *dst, gs_texture_t *src) { UNIMPLEMENTED }
void device_load_swapchain(gs_device_t *device, gs_swapchain_t *swap) { UNIMPLEMENTED }
void device_present(gs_device_t *device) { UNIMPLEMENTED }
