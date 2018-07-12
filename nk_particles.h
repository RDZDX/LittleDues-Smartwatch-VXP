#ifndef NK_PARTICLES_H
#define NK_PARTICLES_H

#include "macro_utils.h"
#define PARTICLE_COUNT 10

double cos(double a);
double sin(double a);

static nk_int 
nk_particles(struct nk_context* ctx, nk_int x, nk_int y, const char* res, nk_int particle_wh, bool* reset) {
	nk_int p;
	double angle, angle_delta;
	static double r;
	static bool done;

	if (*reset) {
		r = 0;
		done = false;
		*reset = false;
	}

	if (r > vm_graphic_get_screen_width()/2) {
		if (!done) {
			done = true;
			return 1;
		}
		return 0;
	}

	angle = 0;
	angle_delta = 360.0f/PARTICLE_COUNT;
	
	nk_layout_row_static(ctx, 0, 0, PARTICLE_COUNT);
	for (p = 0; p < PARTICLE_COUNT; p++) {
		struct nk_image particle;
		particle = nk_image_path(res);
		particle.xo = (short)(x - particle_wh + r * cos(angle * DEG_TO_RAD));
		particle.yo = (short)(y - particle_wh + r * sin(angle * DEG_TO_RAD));
		particle.scale = 200;
		nk_image(ctx, particle);
		angle += angle_delta;
	}
	r += 3.5;
	return 0;
}


#endif
