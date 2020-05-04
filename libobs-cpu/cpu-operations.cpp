
#include "cpu-subsystem.h"

struct BlitParams
{
	gs_texture_t *src;
	gs_texture_t *dst;
	int64_t src_x, src_y, src_width, src_height;
	int64_t dst_x, dst_y, dst_width, dst_height;
};

template<size_t src_bpp, size_t dst_bpp, typename CopyOp>
static void blit_with_op(const CopyOp &op, BlitParams params)
{
	gs_texture_t *src = params.src;
	gs_texture_t *dst = params.dst;
	int64_t src_x = params.src_x;
	int64_t src_y = params.src_y;
	int64_t src_width = params.src_width;
	int64_t src_height = params.src_height;
	int64_t dst_x = params.dst_x;
	int64_t dst_y = params.dst_y;
	int64_t dst_width = params.dst_width;
	int64_t dst_height = params.dst_height;

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

extern "C" void cpu_blit_texture(gs_texture_t *src, gs_texture_t *dst,
					  int64_t src_x, int64_t src_y, int64_t src_width, int64_t src_height,
					  int64_t dst_x, int64_t dst_y, int64_t dst_width, int64_t dst_height)
{
	if(src_x == 0 && src_y == 0 && src_width == src->width && src_height == src->height
	   && dst_x == 0 && dst_y == 0 && dst_width == dst->width && dst_height == dst->height
	   && src->width == dst->width && src->height == dst->height)
	{
		// direct copy
		memcpy(dst->data, src->data, cpu_tex_data_size (dst));
		return;
	}

	BlitParams params = { src, dst, src_x, src_y, src_width, src_height, dst_x, dst_y, dst_width, dst_height };

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