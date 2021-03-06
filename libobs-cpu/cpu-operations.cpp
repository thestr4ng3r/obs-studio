
#include "cpu-subsystem.h"

template<size_t src_bpp, size_t dst_bpp, typename CopyOp>
static void blit_with_op(const CopyOp &op, cpu_blit_params params)
{
	CPU_BLIT_PARAMS_UNPACK

	float scale_x = (float)src_width / (float)dst_width;
	float scale_y = (float)src_height / (float)dst_height;
	int64_t x, y;
	for(y = 0; y < dst_height; y++)
	{
		int64_t dy = dst_y + y;
		if(dy < 0 || dy >= dst->height)
			continue;
		int64_t sy = (int64_t)(y * scale_y) + src_y;
		if (sy >= src->height)
			sy = src->height - 1;
		if (sy < 0)
			sy = 0;
		for(x = 0; x < dst_width; x++)
		{
			int64_t dx = dst_x + x;
			if(dx < 0 || dx >= dst->width)
				continue;
			int64_t sx = (int64_t)(x * scale_x) + src_x;
			if (sx >= src->width)
				sx = src->width - 1;
			if (sx < 0)
				sx = 0;
			// TODO: could optimize this for specific sizes
			// TODO: blending
			// TODO: interpolation
			op(dst->data + (dy * dst->width + dx) * dst_bpp,
				src->data + (sy * src->width + sx) * src_bpp);
		}
	}
}

extern "C" void cpu_blit_texture(struct cpu_blit_params params)
{
	CPU_BLIT_PARAMS_UNPACK

	if(src_x == 0 && src_y == 0 && src_width == src->width && src_height == src->height
	   && dst_x == 0 && dst_y == 0 && dst_width == dst->width && dst_height == dst->height
	   && src->width == dst->width && src->height == dst->height)
	{
		// direct copy
		memcpy(dst->data, src->data, cpu_tex_data_size (dst));
		return;
	}

	switch(src->color_format)
	{
	case GS_RGBA:
		switch(dst->color_format)
		{
		case GS_RGBA:
			blit_with_op<4, 4>([](void *d, void *s){ *(uint32_t *)d = *(uint32_t *)s; }, params);
			return;
		}
		return;
	case GS_BGRX:
		switch(dst->color_format)
		{
		case GS_RGBA:
			blit_with_op<4, 4>([](void *d, void *s){
				uint8_t *ss = (uint8_t *)s;
				uint8_t *dd = (uint8_t *)d;
				dd[0] = ss[2];
				dd[1] = ss[1];
				dd[2] = ss[0];
				dd[3] = 0xff;
			}, params);
			return;
		}
		return;
	}

	blog(LOG_ERROR, "Can't blit between these formats");
}

template<size_t src_bpp, size_t dst_bpp, size_t res_div, typename CopyOp>
static void format_convert_with_op(const CopyOp &op, gs_texture_t *src, gs_texture_t *dst)
{
	size_t x, y;
	for(y = 0; y < dst->height; y++)
	{
		size_t sy = y * res_div;
		for(x = 0; x < dst->width; x++)
		{
			size_t sx = x * res_div;
			op(dst->data + (y * dst->width + x) * dst_bpp,
				src->data + (sy * src->width + sx) * src_bpp);
		}
	}
}

extern "C" void cpu_rgba_to_nv12_y(gs_texture_t *src, gs_texture_t *dst, struct vec4 color_vec0)
{
	if(dst->width != src->width || dst->height != src->height)
	{
		blog(LOG_ERROR, "Resolution mismatch in NV12 Y conversion");
		return;
	}
	if(dst->color_format != GS_R8)
	{
		blog(LOG_ERROR, "Invalid dst format for NV12 Y conversion");
		return;
	}

	if(src->color_format == GS_RGBA)
	{
		vec3 dv;
		dv.x = color_vec0.x;
		dv.y = color_vec0.y;
		dv.z = color_vec0.z;
		float w = color_vec0.w;
		format_convert_with_op<4, 1, 1>([dv, w](uint8_t *d, uint8_t *s){
			vec3 rgb;
			rgb.x = (float)s[0] / 255.0f;
			rgb.y = (float)s[1] / 255.0f;
			rgb.z = (float)s[2] / 255.0f;
			d[0] = (uint8_t)((vec3_dot(&rgb, &dv) + w) * 255.0f);
		}, src, dst);
	}
	else
	{
		blog(LOG_ERROR, "Unsupported dst format for NV12 Y conversion");
		return;
	}
}