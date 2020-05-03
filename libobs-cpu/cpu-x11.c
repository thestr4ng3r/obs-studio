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

#include <X11/Xlib.h>
#include <X11/Xlib-xcb.h>

#include <xcb/xcb.h>

#include <stdio.h>

#include "cpu-subsystem.h"

struct cpu_platform {
	Display *display;
};

struct cpu_windowinfo {
	xcb_window_t window;
	xcb_gcontext_t foreground;
};

static int x_error_handler(Display *display, XErrorEvent *error)
{
	char str1[512];
	char str2[512];
	char str3[512];
	XGetErrorText(display, error->error_code, str1, sizeof(str1));
	XGetErrorText(display, error->request_code, str2, sizeof(str2));
	XGetErrorText(display, error->minor_code, str3, sizeof(str3));

	blog(LOG_ERROR,
		 "X Error: %s, Major opcode: %s, "
		 "Minor opcode: %s, Serial: %lu",
		 str1, str2, str3, error->serial);
	return 0;
}

static Display *open_windowless_display(void)
{
	Display *display = XOpenDisplay(NULL);
	xcb_connection_t *xcb_conn;
	xcb_screen_iterator_t screen_iterator;
	xcb_screen_t *screen;

	if (!display) {
		blog(LOG_ERROR, "Unable to open new X connection!");
		return NULL;
	}

	xcb_conn = XGetXCBConnection(display);
	if (!xcb_conn) {
		blog(LOG_ERROR, "Unable to get XCB connection to main display");
		goto error;
	}

	screen_iterator = xcb_setup_roots_iterator(xcb_get_setup(xcb_conn));
	screen = screen_iterator.data;
	if (!screen) {
		blog(LOG_ERROR, "Unable to get screen root");
		goto error;
	}

	return display;

	error:
	if (display)
		XCloseDisplay(display);
	return NULL;
}

static xcb_get_geometry_reply_t *get_window_geometry(xcb_connection_t *xcb_conn, xcb_drawable_t drawable)
{
	xcb_get_geometry_cookie_t cookie;
	xcb_generic_error_t *error;
	xcb_get_geometry_reply_t *reply;

	cookie = xcb_get_geometry(xcb_conn, drawable);
	reply = xcb_get_geometry_reply(xcb_conn, cookie, &error);

	if (error) {
		blog(LOG_ERROR, "Failed to fetch parent window geometry!");
		free(error);
		free(reply);
		return 0;
	}

	free(error);
	return reply;
}

static xcb_screen_t *get_screen_from_root(xcb_connection_t *xcb_conn,
										  xcb_window_t root)
{
	xcb_screen_iterator_t iter =
			xcb_setup_roots_iterator(xcb_get_setup(xcb_conn));

	while (iter.rem) {
		if (iter.data->root == root)
			return iter.data;

		xcb_screen_next(&iter);
	}

	return 0;
}

struct cpu_platform *cpu_platform_create(gs_device_t *device, uint32_t adapter)
{
	struct cpu_platform *plat = bmalloc(sizeof(struct cpu_platform));
	Display *display = open_windowless_display();

	if (!display) {
		goto fail_display_open;
	}

	XSetEventQueueOwner(display, XCBOwnsEventQueue);
	XSetErrorHandler(x_error_handler);

	plat->display = display;

	goto success;

fail_display_open:
	bfree(plat);
	plat = NULL;
	success:
	UNUSED_PARAMETER(adapter);
	return plat;
}

void cpu_platform_destroy(struct cpu_platform *plat)
{
}

bool cpu_platform_init_swapchain(struct gs_swap_chain *swap)
{
	Display *display = swap->device->plat->display;
	xcb_connection_t *xcb_conn = XGetXCBConnection(display);
	xcb_window_t parent = swap->info.window.id;
	xcb_get_geometry_reply_t *geometry = get_window_geometry(xcb_conn, parent);

	if (!geometry)
		goto beach;

	xcb_screen_t *screen = get_screen_from_root(xcb_conn, geometry->root);
	bool status = false;

	int visual;

	xcb_gcontext_t foreground = xcb_generate_id (xcb_conn);
	uint32_t mask = XCB_GC_FOREGROUND | XCB_GC_GRAPHICS_EXPOSURES;
	uint32_t values[2] = {screen->black_pixel, 0};

	xcb_create_gc (xcb_conn, foreground, screen->root, mask, values);

	xcb_window_t wid = xcb_generate_id(xcb_conn);
	mask = XCB_CW_BACK_PIXEL;
	values[0] = screen->white_pixel;
	xcb_create_window(xcb_conn, 24 /* Hardcoded? */, wid, parent, 0, 0, geometry->width, geometry->height, 0, 0, visual, mask, values);

	xcb_map_window(xcb_conn, wid);
	xcb_flush(xcb_conn);

	swap->wi = bmalloc(sizeof(struct cpu_windowinfo));
	swap->wi->window = wid;
	swap->wi->foreground = foreground;

	status = true;
beach:
	free(geometry);
	return status;
}

void cpu_platform_fini_swapchain(struct gs_swap_chain *swap)
{
	// TODO: more cleanup?
	bfree(swap->wi);
}

void cpu_platform_resize_swapchain(struct gs_swap_chain *swap, uint32_t width, uint32_t height)
{
	Display *display = swap->device->plat->display;
	xcb_connection_t *xcb_conn = XGetXCBConnection(display);
	xcb_window_t window = swap->wi->window;
	uint32_t values[2] = { width, height };
	xcb_configure_window (xcb_conn, window, XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT, values);
}

void cpu_platform_draw(struct gs_device *device)
{
	struct gs_swap_chain *swap = device->swapchain_cur;
	if(!swap)
	{
		blog(LOG_ERROR, "Trying to draw on swapchain without current swapchain");
		return;
	}

	Display *display = swap->device->plat->display;
	xcb_connection_t *xcb_conn = XGetXCBConnection(display);
	xcb_window_t window = swap->wi->window;
	xcb_gcontext_t foreground = swap->wi->foreground;

	/* geometric objects */
	xcb_point_t          points[] = {
			{10, 10},
			{10, 20},
			{20, 10},
			{20, 20}};

	xcb_point_t          polyline[] = {
			{50, 10},
			{ 5, 20},     /* rest of points are relative */
			{25,-20},
			{10, 10}};

	xcb_segment_t        segments[] = {
			{100, 10, 140, 30},
			{110, 25, 130, 60}};

	xcb_rectangle_t      rectangles[] = {
			{ 10, 50, 40, 20},
			{ 80, 50, 10, 40}};

	xcb_arc_t            arcs[] = {
			{10, 100, 60, 40, 0, 90 << 6},
			{90, 100, 55, 40, 0, 270 << 6}};

	xcb_poly_point (xcb_conn, XCB_COORD_MODE_ORIGIN, window, foreground, 4, points);
	xcb_poly_line (xcb_conn, XCB_COORD_MODE_PREVIOUS, window, foreground, 4, polyline);
	xcb_poly_segment (xcb_conn, window, foreground, 2, segments);
	xcb_poly_rectangle (xcb_conn, window, foreground, 2, rectangles);
	xcb_poly_arc (xcb_conn, window, foreground, 2, arcs);

	xcb_flush (xcb_conn);
}