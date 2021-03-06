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

	device->plat = cpu_platform_create(device, adapter);
	if (!device->plat)
		goto fail;

	blog(LOG_INFO, "CPU Renderer loaded.");

	*p_device = device;
	return GS_SUCCESS;

fail:
	blog(LOG_ERROR, "device_create (CPU) failed");
	bfree(device);

	*p_device = NULL;
	return errorcode;
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

static gs_shader_t *create_shader(gs_device_t *device, const char *file)
{
	gs_shader_t *r = bzalloc(sizeof(gs_shader_t));
	r->device = device;
	r->file = bstrdup (file);
	if (!strcmp (file, "share/obs/libobs/default.effect (Vertex shader, technique Draw, pass 0)"))
		r->kind = CPU_SHADER_DEFAULT_DRAW_VERTEX;
	else if (!strcmp (file, "share/obs/libobs/default.effect (Pixel shader, technique Draw, pass 0)"))
		r->kind = CPU_SHADER_DEFAULT_DRAW_FRAGMENT;
	else if (!strcmp (file, "share/obs/libobs/default.effect (Vertex shader, technique DrawAlphaDivide, pass 0)"))
		r->kind = CPU_SHADER_DEFAULT_DRAW_ALPHA_DIVIDE_VERTEX;
	else if (!strcmp (file, "share/obs/libobs/default.effect (Pixel shader, technique DrawAlphaDivide, pass 0)"))
		r->kind = CPU_SHADER_DEFAULT_DRAW_ALPHA_DIVIDE_FRAGMENT;
	else if (!strcmp (file, "share/obs/libobs/opaque.effect (Vertex shader, technique Draw, pass 0)"))
		r->kind = CPU_SHADER_OPAQUE_VERTEX;
	else if (!strcmp (file, "share/obs/libobs/opaque.effect (Pixel shader, technique Draw, pass 0)"))
		r->kind = CPU_SHADER_OPAQUE_FRAGMENT;
	else if (!strcmp (file, "share/obs/libobs/solid.effect (Vertex shader, technique Solid, pass 0)"))
		r->kind = CPU_SHADER_SOLID_VERTEX;
	else if (!strcmp (file, "share/obs/libobs/solid.effect (Pixel shader, technique Solid, pass 0)"))
		r->kind = CPU_SHADER_SOLID_FRAGMENT;
	else if (!strcmp (file, "share/obs/libobs/solid.effect (Vertex shader, technique SolidColored, pass 0)"))
		r->kind = CPU_SHADER_SOLID_COLORED_VERTEX;
	else if (!strcmp (file, "share/obs/libobs/solid.effect (Pixel shader, technique SolidColored, pass 0)"))
		r->kind = CPU_SHADER_SOLID_COLORED_FRAGMENT;
	else if (!strcmp (file, "share/obs/libobs/solid.effect (Vertex shader, technique Random, pass 0)"))
		r->kind = CPU_SHADER_SOLID_RANDOM_VERTEX;
	else if (!strcmp (file, "share/obs/libobs/solid.effect (Pixel shader, technique Random, pass 0)"))
		r->kind = CPU_SHADER_SOLID_RANDOM_FRAGMENT;
	else if (!strcmp (file, "share/obs/libobs/format_conversion.effect (Vertex shader, technique NV12_Y, pass 0)"))
		r->kind = CPU_SHADER_FORMAT_CONVERSION_NV12_Y_VERTEX;
	else if (!strcmp (file, "share/obs/libobs/format_conversion.effect (Pixel shader, technique NV12_Y, pass 0)"))
		r->kind = CPU_SHADER_FORMAT_CONVERSION_NV12_Y_FRAGMENT;
	else if (!strcmp (file, "share/obs/libobs/format_conversion.effect (Vertex shader, technique NV12_UV, pass 0)"))
		r->kind = CPU_SHADER_FORMAT_CONVERSION_NV12_UV_VERTEX;
	else if (!strcmp (file, "share/obs/libobs/format_conversion.effect (Pixel shader, technique NV12_UV, pass 0)"))
		r->kind = CPU_SHADER_FORMAT_CONVERSION_NV12_UV_FRAGMENT;
	else
	{
		blog(LOG_WARNING, "Shader unknown to CPU Subsystem: %s", file);
		r->kind = CPU_SHADER_UNKNOWN;
	}
	return r;
}

gs_shader_t *device_vertexshader_create(gs_device_t *device, const char *shader, const char *file, char **error_string)
{
	return create_shader(device, file);
}

gs_shader_t *device_pixelshader_create(gs_device_t *device, const char *shader, const char *file, char **error_string)
{
	return create_shader(device, file);
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
	if(!shader)
		return;
	bfree(shader->file);
	bfree(shader);
}

gs_sparam_t *gs_shader_get_param_by_name(gs_shader_t *shader, const char *name)
{
	gs_sparam_t *r = bzalloc(sizeof(gs_sparam_t));
	r->device = shader->device;
	r->shader = shader;
	r->name = bstrdup(name);
	r->kind = CPU_SHADER_PARAM_UNKNOWN;
	if(shader->kind == CPU_SHADER_DEFAULT_DRAW_FRAGMENT)
	{
		if(!strcmp(name, "image"))
			r->kind = CPU_SHADER_PARAM_IMAGE;
	}
	else if(shader->kind == CPU_SHADER_FORMAT_CONVERSION_NV12_Y_FRAGMENT
			|| shader->kind == CPU_SHADER_FORMAT_CONVERSION_NV12_UV_FRAGMENT)
	{
		if(!strcmp(name, "image"))
			r->kind = CPU_SHADER_PARAM_IMAGE;
		else if(!strcmp(name, "color_vec0"))
			r->kind = CPU_SHADER_PARAM_COLOR_VEC0;
		else if(!strcmp(name, "color_vec1"))
			r->kind = CPU_SHADER_PARAM_COLOR_VEC1;
		else if(!strcmp(name, "color_vec2"))
			r->kind = CPU_SHADER_PARAM_COLOR_VEC2;
	}

	if(r->kind == CPU_SHADER_PARAM_UNKNOWN)
	{
		blog(LOG_WARNING, "Unknown param for shader %s: %s", shader->file, name);
	}
	return r;
}

void gs_shader_get_param_info(const gs_sparam_t *param, struct gs_shader_param_info *info)
{
	info->name = param->name;
	switch(param->kind)
	{
	case CPU_SHADER_PARAM_IMAGE:
		info->type = GS_SHADER_PARAM_TEXTURE;
		break;
	case CPU_SHADER_PARAM_COLOR_VEC0:
	case CPU_SHADER_PARAM_COLOR_VEC1:
	case CPU_SHADER_PARAM_COLOR_VEC2:
		info->type = GS_SHADER_PARAM_VEC4;
		break;
	case CPU_SHADER_PARAM_UNKNOWN:
		info->type = GS_SHADER_PARAM_UNKNOWN;
		break;
	}
}

void gs_shader_set_val(gs_sparam_t *param, const void *val, size_t size)
{
	switch(param->kind) {
	case CPU_SHADER_PARAM_IMAGE:
		param->device->params.image = *(gs_texture_t **)val;
		break;
	case CPU_SHADER_PARAM_COLOR_VEC0:
		param->device->params.color_vec0 = *(struct vec4 *)val;
		break;
	case CPU_SHADER_PARAM_COLOR_VEC1:
		param->device->params.color_vec1 = *(struct vec4 *)val;
		break;
	case CPU_SHADER_PARAM_COLOR_VEC2:
		param->device->params.color_vec2 = *(struct vec4 *)val;
		break;
	case CPU_SHADER_PARAM_UNKNOWN:
		break;
	}
}

void gs_shader_set_texture(gs_sparam_t *param, gs_texture_t *val)
{
	switch(param->kind) {
	case CPU_SHADER_PARAM_IMAGE:
		param->device->params.image = val;
		break;
	default:
		blog(LOG_ERROR, "Tried to set texture to an invalid param %s", param->name);
		break;
	}
}

gs_samplerstate_t *device_samplerstate_create(gs_device_t *device, const struct gs_sampler_info *info)
{
	return bzalloc(sizeof(gs_samplerstate_t));
}

void gs_samplerstate_destroy(gs_samplerstate_t *samplerstate)
{
	bfree(samplerstate);
}

size_t cpu_tex_data_size(gs_texture_t *tex)
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
	size_t size = cpu_tex_data_size(r);
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

bool gs_texture_map(gs_texture_t *tex, uint8_t **ptr, uint32_t *linesize)
{
	*ptr = tex->data;
	*linesize = tex->width * gs_get_format_bpp(tex->color_format) / 8;
	return true;
}

void gs_texture_unmap(gs_texture_t *tex)
{
}

enum gs_color_format gs_texture_get_color_format(const gs_texture_t *tex)
{
	return tex->color_format;
}

gs_stagesurf_t *device_stagesurface_create(gs_device_t *device, uint32_t width, uint32_t height, enum gs_color_format color_format)
{
	gs_stagesurf_t *r = bzalloc(sizeof(gs_stagesurf_t));
	r->tex = device_texture_create(device, width, height, color_format, 1, NULL, 0);
	return r;
}

void gs_stagesurface_destroy(gs_stagesurf_t *stagesurf)
{
	if(!stagesurf)
		return;
	gs_texture_destroy(stagesurf->tex);
	bfree(stagesurf);
}

void device_stage_texture(gs_device_t *device, gs_stagesurf_t *dst, gs_texture_t *src)
{
	struct cpu_blit_params params = {
		.src = src,
		.dst = dst->tex,
		.src_x = 0,
		.src_y = 0,
		.src_width = src->width,
		.src_height = src->height,
		.dst_x = 0,
		.dst_y = 0,
		.dst_width = dst->tex->width,
		.dst_height = dst->tex->height
	};
	cpu_blit_texture(params);
}

uint32_t gs_stagesurface_get_width(const gs_stagesurf_t *stagesurf)
{
	return stagesurf->tex->width;
}

uint32_t gs_stagesurface_get_height(const gs_stagesurf_t *stagesurf)
{
	return stagesurf->tex->height;
}

enum gs_color_format gs_stagesurface_get_color_format(const gs_stagesurf_t *stagesurf)
{
	return stagesurf->tex->color_format;
}

bool gs_stagesurface_map(gs_stagesurf_t *stagesurf, uint8_t **data, uint32_t *linesize)
{
	return gs_texture_map(stagesurf->tex, data, linesize);
}

void gs_stagesurface_unmap(gs_stagesurf_t *stagesurf)
{
	gs_texture_unmap(stagesurf->tex);
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

gs_texture_t *device_get_render_target(const gs_device_t *device)
{
	return device->render_target.tex;
}

gs_zstencil_t *device_get_zstencil_target(const gs_device_t *device)
{
	return device->render_target.zstencil;
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

void device_projection_push(gs_device_t *device)
{
	da_push_back(device->proj_stack, &device->cur_proj);
}

void device_projection_pop(gs_device_t *device)
{
	struct matrix4 *end;
	if (!device->proj_stack.num)
		return;

	end = da_end(device->proj_stack);
	device->cur_proj = *end;
	da_pop_back(device->proj_stack);
}

void device_set_viewport(gs_device_t *device, int x, int y, int width, int height)
{
	device->viewport.x = x;
	device->viewport.y = y;
	device->viewport.width = width;
	device->viewport.height = height;
}

void device_get_viewport(const gs_device_t *device, struct gs_rect *rect)
{
	rect->x = device->viewport.x;
	rect->y = device->viewport.y;
	rect->cx = device->viewport.width;
	rect->cy = device->viewport.height;
}


static void cpu_vertex_to_screen(gs_device_t *device, int64_t *x_out, int64_t *y_out, struct vec3 *v_in)
{
	struct matrix4 mvp;
	gs_matrix_get(&mvp);
	matrix4_mul(&mvp, &mvp, &device->cur_proj);

	struct vec4 v;
	vec4_from_vec3(&v, v_in);
	vec4_transform(&v, &v, &mvp);
	vec4_divf(&v, &v, v.w);
	vec4_addf(&v, &v, 1.0f);
	vec4_mulf(&v, &v, 0.5f);

	*x_out = (int64_t)(v.x * (float)device->viewport.width) + (int64_t)device->viewport.x;
	*y_out = (int64_t)((1.0f - v.y) * (float)device->viewport.height) + (int64_t)device->viewport.y;
}

static void cpu_draw_blit(gs_device_t *device, enum gs_draw_mode draw_mode, uint32_t start_vert, uint32_t num_verts)
{
	gs_texture_t *src = device->params.image;
	if(!src)
	{
		blog(LOG_ERROR, "No texture bound");
		return;
	}

	gs_texture_t *dst = device->render_target.tex;

	gs_vertbuffer_t *vbo = device->vertex_buffer_cur;
	if(!vbo || vbo->data->num != 4 || !vbo->data->points || draw_mode != GS_TRISTRIP
		|| vbo->data->num_tex != 1 || !vbo->data->tvarray || vbo->data->tvarray[0].width != 2)
	{
		blog(LOG_ERROR, "This doesn't look like a blit, no idea what to do");
		return;
	}

	struct matrix4 mvp;
	gs_matrix_get(&mvp);
	matrix4_mul(&mvp, &mvp, &device->cur_proj);

	struct cpu_blit_params params = { 0 };
	params.src = src;
	params.dst = dst;

	cpu_vertex_to_screen(device, &params.dst_x, &params.dst_y, &vbo->data->points[0]);
	int64_t dst_x1, dst_y1;
	cpu_vertex_to_screen(device, &dst_x1, &dst_y1, &vbo->data->points[3]);
	params.dst_width = dst_x1 - params.dst_x;
	params.dst_height = dst_y1 - params.dst_y;

	struct vec2 *uvs = vbo->data->tvarray[0].array;
	struct vec2 uv_min = uvs[0];
	struct vec2 uv_max = uvs[3];

	params.src_x = uv_min.x;
	params.src_y = uv_min.y;
	params.src_width = uv_max.x - uv_min.x;
	params.src_height = uv_max.y - uv_min.y;

	if(dst)
	{
		// texture -> texture
		cpu_blit_texture(params);
	}
	else
	{
		// texture -> swapchain
		cpu_platform_blit(device, params);
	}

}

static void cpu_draw_format_conversion(gs_device_t *device, enum gs_draw_mode draw_mode, uint32_t start_vert, uint32_t num_verts)
{
	gs_texture_t *src = device->params.image;
	if(!src)
	{
		blog(LOG_ERROR, "No texture bound");
		return;
	}

	gs_texture_t *dst = device->render_target.tex;
	if(!dst)
	{
		blog(LOG_ERROR, "No render target bound for format conversion");
		return;
	}

	if(device->vertex_shader_cur->kind == CPU_SHADER_FORMAT_CONVERSION_NV12_Y_VERTEX)
	{
		cpu_rgba_to_nv12_y(src, dst, device->params.color_vec0);
	}
	else if(device->vertex_shader_cur->kind == CPU_SHADER_FORMAT_CONVERSION_NV12_UV_VERTEX)
	{
		blog(LOG_ERROR, "TODO: Convert UV: %u %u -> %u %u", src->width, src->height, dst->width, dst->height);
	}
}

void device_draw(gs_device_t *device, enum gs_draw_mode draw_mode, uint32_t start_vert, uint32_t num_verts)
{
	gs_effect_t *effect = gs_get_effect();
	if (effect)
		gs_effect_update_params(effect);

	gs_shader_t *vert = device->vertex_shader_cur;
	gs_shader_t *frag = device->fragment_shader_cur;
	if(!vert || !frag)
	{
		blog(LOG_ERROR, "Vertex or Fragment Shader not set");
		return;
	}

	if(vert->kind == CPU_SHADER_DEFAULT_DRAW_VERTEX && frag->kind == CPU_SHADER_DEFAULT_DRAW_FRAGMENT)
		cpu_draw_blit(device, draw_mode, start_vert, num_verts);
	else if((vert->kind == CPU_SHADER_FORMAT_CONVERSION_NV12_Y_VERTEX && frag->kind == CPU_SHADER_FORMAT_CONVERSION_NV12_Y_FRAGMENT)
		|| (vert->kind == CPU_SHADER_FORMAT_CONVERSION_NV12_UV_VERTEX && frag->kind == CPU_SHADER_FORMAT_CONVERSION_NV12_UV_FRAGMENT))
		cpu_draw_format_conversion(device, draw_mode, start_vert, num_verts);
	else if(vert->kind == CPU_SHADER_UNKNOWN || frag->kind == CPU_SHADER_UNKNOWN)
		blog(LOG_ERROR, "Vertex or Fragment Shader unknown: %s + %s", vert->file, frag->file);
	else
		blog(LOG_ERROR, "Vertex or Fragment Shader unimplemented: %s + %s", vert->file, frag->file);
}

gs_swapchain_t *device_swapchain_create(gs_device_t *device, const struct gs_init_data *info)
{
	struct gs_swap_chain *swap = bzalloc(sizeof(struct gs_swap_chain));

	swap->device = device;
	swap->info = *info;

	if(!cpu_platform_init_swapchain(swap))
	{
		blog(LOG_ERROR, "cpu_platform_init_swapchain failed");
		gs_swapchain_destroy(swap);
		return NULL;
	}

	return swap;
}

void gs_swapchain_destroy(gs_swapchain_t *swapchain)
{
	if (!swapchain)
		return;

	if (swapchain->device->swapchain_cur == swapchain)
		device_load_swapchain(swapchain->device, NULL);

	cpu_platform_fini_swapchain(swapchain);
	bfree(swapchain);
}

void device_load_swapchain(gs_device_t *device, gs_swapchain_t *swap)
{
	device->swapchain_cur = swap;
}

void device_get_size(const gs_device_t *device, uint32_t *cx, uint32_t *cy)
{
	*cx = device->width;
	*cy = device->height;
}

void device_resize(gs_device_t *device, uint32_t cx, uint32_t cy)
{
	device->width = cx;
	device->height = cy;
	if(!device->swapchain_cur)
	{
		blog(LOG_ERROR, "No current swapchain");
		return;
	}
	cpu_platform_resize_swapchain(device->swapchain_cur, cx, cy);
}

void device_present(gs_device_t *device)
{
	// TODO
}


void *device_get_device_obj(gs_device_t *device) { UNIMPLEMENTED_RET(NULL) }
uint32_t device_get_width(const gs_device_t *device) { UNIMPLEMENTED_RET(0) }
uint32_t device_get_height(const gs_device_t *device) { UNIMPLEMENTED_RET(0) }
gs_timer_t *device_timer_create(gs_device_t *device) { UNIMPLEMENTED_RET(NULL) }
gs_timer_range_t *device_timer_range_create(gs_device_t *device) { UNIMPLEMENTED_RET(NULL) }
void device_load_texture(gs_device_t *device, gs_texture_t *tex, int unit) { UNIMPLEMENTED }
void device_load_samplerstate(gs_device_t *device, gs_samplerstate_t *ss, int unit) { UNIMPLEMENTED }
void device_load_default_samplerstate(gs_device_t *device, bool b_3d, int unit) { UNIMPLEMENTED }
gs_shader_t *device_get_vertex_shader(const gs_device_t *device) { UNIMPLEMENTED_RET(NULL) }
gs_shader_t *device_get_pixel_shader(const gs_device_t *device) { UNIMPLEMENTED_RET(NULL) }
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
void device_set_scissor_rect(gs_device_t *device, const struct gs_rect *rect) { UNIMPLEMENTED }
void device_frustum(gs_device_t *device, float left, float right, float top, float bottom, float near, float far) { UNIMPLEMENTED }
void device_debug_marker_begin(gs_device_t *device, const char *markername, const float color[4]) { UNIMPLEMENTED }
void device_debug_marker_end(gs_device_t *device) {	UNIMPLEMENTED }

gs_texture_t *device_cubetexture_create(gs_device_t *device, uint32_t size, enum gs_color_format color_format,
		uint32_t levels, const uint8_t **data, uint32_t flags) { UNIMPLEMENTED_RET(NULL) }
gs_texture_t *device_voltexture_create(gs_device_t *device, uint32_t width, uint32_t height, uint32_t depth,
		enum gs_color_format color_format, uint32_t levels, const uint8_t *const *data, uint32_t flags) { UNIMPLEMENTED_RET(NULL) }
gs_zstencil_t *device_zstencil_create(gs_device_t *device, uint32_t width, uint32_t height, enum gs_zstencil_format format) { UNIMPLEMENTED_RET(NULL) }
gs_indexbuffer_t *device_indexbuffer_create(gs_device_t *device, enum gs_index_type type, void *indices, size_t num, uint32_t flags) { UNIMPLEMENTED_RET(NULL) }