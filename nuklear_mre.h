/*
	Nuklear MRE Implementation: nuklear_mre.h 
	Author: Varuna Singh, UstadMobile
*/

#include "nuklear.h"
#include "mre_components.h"
#include "sprites.h"
#include "share.h"
#include "vmlog.h"
#include "vmres.h"

//#define MRE_STR_SIZE_MAX 50

/* ==============================================================
 *								API
 * =============================================================*/

/* If Implementation Header file is set: If declaration done
	Note: This will always be called at the start. 
	As you can see if NK_MRE_H_ will be	defined only when not 
	set. So these functions will be declared only once.
*/ 
#ifndef NK_MRE_H_
	#define NK_MRE_H_ 

	struct mre_view_p* mre_view;

	void initiate_nk_gui(void);
	void update_gui();

	/*	Define Platform specific preprocessor directive (#define) macros.
		eg: #define WIN32_LEAN_AND_MEAN 
		This "reduces the size of the Win32 header files by excluding 
		some of the less frequently used APIs" -is Windows GDI 
		specific. Ignoring.

		Here you can also define some platform specific header files relevant.
	*/

	/*	Nuklear Header File 
		In new version v1.05, the nuklear.h file is included in main.c
		#include "nuklear.h"
	*/

	/* Font */
	typedef struct MreFont MreFont; //Gotta do this for it to know it exists
	NK_API MreFont* nk_mrefont_create(const char *name, int size);
	NK_API void nk_mre_set_font();
	NK_API void nk_mrefont_del();
	
	/* Declare the implementation:*/
	NK_API struct nk_context* nk_mre_init(struct nk_user_font *font,unsigned int width, unsigned int height);
	NK_API void nk_mre_handle_sys_event(VMINT message, VMINT param);
	NK_API void nk_mre_handle_key_event(VMINT message, VMINT keycode);
	NK_API void nk_mre_handle_pen_event(VMINT event, VMINT x, VMINT y);
	NK_API void nk_mre_render(struct nk_color clear);
	NK_API void nk_mre_shutdown(void);

	/*Platform specific*/
	NK_API int nk_init_mre(struct nk_context *ctx, const struct nk_user_font *font);
	NK_INTERN void* mre_malloc(nk_handle unused, void *old, nk_size size); //nk_handle: A pointer, an int id; nk_size: unsigned long
	NK_INTERN void mre_free(nk_handle unused, void* old); //nk_handle: A pointer, an int id

#endif

/* ==============================================================
 *						MRE IMPLEMENTATION
 * =============================================================*/

/*TODO: Remove*/
#define NK_MRE_IMPLEMENTATION

/*if set to use MRE implementation*/
#ifdef NK_MRE_IMPLEMENTATION	

	/*********************
		MRE HEADER FILES
	*********************/
	#include "vmsys.h"


	/*********************
			STRUCTS
	*********************/
	/*	
		Font object
		MRE Doesn't really have a Font object
	*/
	struct MreFont{
		struct nk_user_font nk;
		int size;
	};

	struct FocusableObjects{
		int id;
		struct nk_rect area;
	};

	struct mre mre;

	/*
	MRE-Nuklear Components
	Components used to draw
	TODO: Moved to mre_components. Check, Test and Delete this.
	*/
	struct mre_nk_c_old {
		int id;
		char *type;
		char *title;
		int len;
		struct nk_style style;
		int hovering;
		int is_hoverable;
		/* For Video: */ 
		char *url;
		char *thumbnail;
	};

	/* MRE View Properties 
		TODO: Moved to mre_components. Check, Test and Delete this.
	*/ 
	struct mre_view_p_old{
		int hoverable_counter;
		int hoverable_items;
		struct mre_nk_c **components;
		//struct mre_nk_c * components;
		int components_count;
	}mre_view_old;


	/*********************
		  FUNCTIONS
	*********************/

	/*	MRE's malloc 
	*/
	NK_INTERN void* mre_malloc(nk_handle unused, void *old, nk_size size){

		/*So basically what malloc does is set some memory aside in the, well, memory
		and pass it on to the pointer back. That way we know for sure that memory is
		allocated and you can store whatever data you want in it (puppy pics).
		Its important to allocate the right amount of memory needed. If an object 
		has multiple variables, we need to calculate it and allocate it. Its really
		bad to allocate a riddiculous huge amount of size that will never get filled.
		Not only that but in memory sensitive devices this will unneccesarily crash.
		Malloc will NOT initialise the memory with some data. Thats what calloc does. 
		calloc will initialise with 0s. Malloc just keeps it ready. Some cases we 
		would want to have some data but not here (afaik).
		This method gets in:
			a. nk_handle unused : Has a pointer and an int id
			b. void *old:	Old 
			c. nk_size size: The size of memory allocation in unsigned long
	
		Steps:
			i. Cast void to both unused and old
			ii. Malloc size and return it.
		*/

		void *malloc_pointer;
		
		/* Not sure what this does. Ignoring for now*/
		//NK_UNUSED(unused);
		//NK_UNUSED(old);

		//TODO: Check this for MRE phones
		//malloc_pointer = (void *)vm_malloc(size);
		malloc_pointer = (void *)vm_malloc(size); 
		return malloc_pointer;
		
	}

	/*	MRE's free
	*/
	NK_INTERN void mre_free(nk_handle unused, void *ptr){
		NK_UNUSED(unused);
		vm_free(ptr);
	}

	/*
		Initialises the context - MRE way
		We have to call this instead of nk_init_default (which is part of nuklear..h)
		because that method uses STL and we have to make our own implementation of it	
	*/
	NK_API int
	nk_init_mre(struct nk_context *ctx, const struct nk_user_font *font) //similar to nk_init_default
	{
		/*
			nk_allocator:
				a. nk_alloc Obj: userdata
					i. A pointer
					ii. An id
				b. alloc ()
				c. free ()

			Steps:
			1. Set the pointer to a value
			2. Set alloc method
			3. Set Free Method
			4. Call nk_init(ctx, font) to begin process

		*/

		struct nk_allocator alloc;
		alloc.userdata.ptr = 0;
		/*	Assign implementation's alloc and malloc
			alloc(nk_handle, void *old, nk_size)
			free(nk_handle, void*) 
			Make sure they work OK
		*/ 
		alloc.alloc = mre_malloc;
		alloc.free = mre_free;

		//NOT ALLOWING FONT TO BE SET HERE. 
		//return nk_init(ctx, &alloc, font);


		return nk_init(ctx, &alloc, font);

		/*Origi code: 
		struct nk_allocator alloc;
		alloc.userdata.ptr = 0;
		alloc.alloc = nk_malloc; //(nk_handle, void *old, nk_size);
		alloc.free = nk_mfree;   //(nk_handle, void*)
		return nk_init(ctx, &alloc, font);*/
	}

	/*
		Convert nuklear's color into implementation color!
		 Color being used for all that drawing we gotta do
		 Used internally, not an implementation 
		 Can be called anything..
	*/
	static VMINT 
	convert_color(struct nk_color c){
		//..convert nk_color c to MRE color..
		//returning red for now..
		//return VM_RGB32_ARGB(c.a, c.r, c.g, c.b);
		return VM_RGB565_ARGB(c.a, c.r, c.g, c.b);
		//return VM_COLOR_RED;
	}


	/***********************************************
		IMPL FUNCTIONS

		Stroke: is line drawing
		Fill: is line drawing + the area under the 
				outline (usually color)
	***********************************************/

	/*
		Scissor
		Something to do with scissor ? 
		Called by this command: NK_COMMAND_SCISSOR
	*/
	static void 
	nk_mre_scissor(void){
		vm_log_debug("Scissor-ing..");
	}

	/*
		Stroke line
		Draw a line as stroke (outline)
		Called by command: NK_COMMAND_LINE
	*/ 
	static void
	nk_mre_stroke_line(short x0, short y0, short x1, short y1, 
						unsigned int line_thickness, struct nk_color col)
	{
		VMUINT8 *buffer;
		VMUINT16 color;
		VMINT LINE_X1, LINE_X2, LINE_Y1, LINE_Y2;
		//Have the layer handle ready.. layer_hdl
		//Maybe as global or passed here..

		if (col.a == 0) return;

		/* 
			get the target buffer from the layer
		*/
		buffer = vm_graphic_get_layer_buffer(layer_hdl[0]);

		//No need to fill a rectangle here..
		//vm_graphic_fill_rect(buffer, MRE_SET_SRC_LEFT_TOP_X, MRE_SET_SRC_LEFT_TOP_Y, vm_graphic_get_screen_width(), 
		//					 vm_graphic_get_screen_height(), VM_COLOR_WHITE, VM_COLOR_GREEN);

		color = VM_COLOR_RED;
		color = convert_color(col);
		LINE_X1 = x0; 
		LINE_X2 = x1;
		LINE_Y1 = y0;
		LINE_Y2 = y1;
		//line_thickness not implemented..

		vm_graphic_line(buffer, LINE_X1, LINE_Y1, LINE_X2, LINE_Y2, color);
		/* 
			Gotta flush 
			Flush updates the LCD
		*/
		//vm_graphic_flush_layer(layer_hdl, 1);
		vm_log_debug("draw line fun ends");
	}

	/*
		Stroke Rectangle
		Draw a rectangle as a stroke (outline)
		Called by: NK_COMMAND_RECT
	*/
	static void
	nk_mre_stroke_rect(short x, short y, unsigned short w,
		unsigned short h, unsigned short r, unsigned short line_thickness, struct nk_color col)
	{
		VMUINT8 *buffer;
		VMUINT16 color;
		VMINT REC_X, REC_Y, REC_W, REC_L;

		if (col.a == 0) return;

		color = convert_color(col);
		REC_X =	x;
		REC_Y = y;
		REC_W = w;
		REC_L = h;

		/*Get the target buffer */
		buffer = vm_graphic_get_layer_buffer(layer_hdl[0]);

		//vm_graphic_fill_rect(buffer, MRE_SET_SRC_LEFT_TOP_X, MRE_SET_SRC_LEFT_TOP_Y, vm_graphic_get_screen_width(), 
		//                 vm_graphic_get_screen_height(), VM_COLOR_WHITE, VM_COLOR_BLUE);

		vm_graphic_rect(buffer, REC_X, REC_Y, REC_W, REC_L, color);
		/* 
			Gotta flush:
			Flush updates the LCD 
		*/
		//vm_graphic_flush_layer(layer_hdl, 1); 

	}

	/*
		Fill Rectangle
		Draws a rectangle outline and color fills it
		Called by command: NK_COMMAND_RECT_FILLED
	*/
	static void
	nk_mre_fill_rect(short x, short y, unsigned short w,
		unsigned short h, unsigned short r, struct nk_color col)
	{
		VMUINT8 *buffer;
		VMUINT16 color;
		VMINT REC_X, REC_Y, REC_W, REC_L;

		if (col.a == 0) return;

		color = convert_color(col);
		REC_X =	x;
		REC_Y = y;
		REC_W = w;
		REC_L = h;

		/*Get the target buffer */
		buffer = vm_graphic_get_layer_buffer(layer_hdl[0]);

		//vm_graphic_fill_rect(buffer, MRE_SET_SRC_LEFT_TOP_X, MRE_SET_SRC_LEFT_TOP_Y, vm_graphic_get_screen_width(), 
		//                 vm_graphic_get_screen_height(), VM_COLOR_WHITE, VM_COLOR_BLUE);

		//vm_graphic_rect(buffer, REC_X, REC_Y, REC_W, REC_L, color);
		vm_graphic_fill_rect(buffer, REC_X, REC_Y, REC_W, REC_L, color, color);

		//Ignoring filled one first
		//vm_graphic_fill_rect(buffer, REC_X, REC_Y, REC_W, REC_L, color, color);
		/* 
			Gotta flush:
			Flush updates the LCD 
		*/
		//vm_graphic_flush_layer(layer_hdl, 1); 

	}

	/*
		Fill Triangle
		Draws a triangle and fills area in between with color
		Called by command: NK_COMMAND_TRIANGLE_FILLED
	*/
	static void
	nk_mre_fill_triangle(short x0, short y0, short x1, short y1,
		short x2, short y2, struct nk_color colr)
	{
		VMUINT8 *buffer;
		VMUINT16 color;
		VMINT TRN_X1, TRN_X2, TRN_X3, TRN_Y1, TRN_Y2, TRN_Y3;
		vm_graphic_color col;

		//vm_graphic_color col;
		vm_graphic_point points[3];

		color = convert_color(colr);
		TRN_X1 = x0;
		TRN_X2 = x1;
		TRN_X3 = x2;
		TRN_Y1 = y0;
		TRN_Y2 = y1;
		TRN_Y3 = y2;

		/* function body */
		vm_log_debug("draw triangle starts");
		points[0].x = TRN_X1;
		points[0].y = TRN_Y1;
		points[1].x = TRN_X2;
		points[1].y = TRN_Y2;
		points[2].x = TRN_X3;
		points[2].y = TRN_Y3;
	   
		col.vm_color_565 = VM_COLOR_RED;
		col.vm_color_888 = VM_COLOR_565_TO_888(VM_COLOR_RED);
		/* sets global color */
		vm_graphic_setcolor(&col);

		/* get the target buffer*/
		buffer = vm_graphic_get_layer_buffer(layer_hdl[0]);

		//vm_graphic_fill_rect(buffer, MRE_SET_SRC_LEFT_TOP_X, MRE_SET_SRC_LEFT_TOP_Y, vm_graphic_get_screen_width(), 
		//					 vm_graphic_get_screen_height(), VM_COLOR_WHITE, VM_COLOR_GREEN);

		/* Draw 3 points from points on the layer */
		vm_graphic_fill_polygon(layer_hdl[0], points, 3);

		/* Gotta flush */
		//vm_graphic_flush_layer(layer_hdl, 1);

	}

	/*	
		Stroke Triangle
		Draws the outline of the triangle
		Called by command: NK_COMMAND_TRIANGLE
	*/
	static void
	nk_mre_stroke_triangle(short x0, short y0, short x1, short y1,
		short x2, short y2, unsigned short line_thickness, struct nk_color colr)
	{
		VMUINT8 *buffer;
		VMUINT16 color;
		VMINT TRN_X1, TRN_X2, TRN_X3, TRN_Y1, TRN_Y2, TRN_Y3;

		vm_graphic_color col;
		vm_graphic_point points[3];

		color = convert_color(colr);
		TRN_X1 = x0;
		TRN_X2 = x1;
		TRN_X3 = x2;
		TRN_Y1 = y0;
		TRN_Y2 = y1;
		TRN_Y3 = y2;

		/* function body */
		vm_log_debug("draw triangle starts");
		points[0].x = TRN_X1;
		points[0].y = TRN_Y1;
		points[1].x = TRN_X2;
		points[1].y = TRN_Y2;
		points[2].x = TRN_X3;
		points[2].y = TRN_Y3;
	   
		col.vm_color_565 = VM_COLOR_RED;
		col.vm_color_888 = VM_COLOR_565_TO_888(VM_COLOR_RED);

		/* sets global color */
		//vm_graphic_setcolor(&col);

		/* get the target buffer*/
		buffer = vm_graphic_get_layer_buffer(layer_hdl[0]);

		//vm_graphic_fill_rect(buffer, MRE_SET_SRC_LEFT_TOP_X, MRE_SET_SRC_LEFT_TOP_Y, vm_graphic_get_screen_width(), 
		//					 vm_graphic_get_screen_height(), VM_COLOR_WHITE, VM_COLOR_GREEN);

		/* Draw 3 points from points on the layer */
		vm_graphic_fill_polygon(layer_hdl[0], points, 3);

		/* Gotta flush */
		//vm_graphic_flush_layer(layer_hdl, 1);

	}

	/*
		Fill Polygon
		Draws polygon with count number from points and fills color in 
		Called by command: NK_COMMAND_POLYGON_FILLED
	*/
	static void
	nk_mre_fill_polygon(const struct nk_vec2i *pnts, int count, struct nk_color colr)
	{
		VMUINT8 *buffer;
		VMUINT16 color;
		
		/*int a=42;
		const int b=a;
		int ipoints[b]; //Wont work because C89
		*/

		//int *a = new int[count]; //Not C syntax. More like c++
		//vm_graphic_point points[10]; //Works because number is known at the time of compilation

		//The way dynamically loading works:
		//char* utf8 = malloc(utf8size);
		int pointsize = count * sizeof(vm_graphic_point);
		vm_graphic_point* points = vm_malloc(pointsize);

		//VMINT TRN_X1, TRN_X2, TRN_X3, TRN_Y1, TRN_Y2, TRN_Y3;

		vm_graphic_color col;
		
		color = convert_color(colr);
		/*TRN_X1 = x0;
		TRN_X2 = x1;
		TRN_X3 = x2;
		TRN_Y1 = y0;
		TRN_Y2 = y1;
		TRN_Y3 = y2;*/

		/* function body */
		vm_log_debug("draw polygon starts");

		/*for every point in nk_vec2i pnts .. put value in points..*/

		/*points[0].x = TRN_X1;
		points[0].y = TRN_Y1;
		points[1].x = TRN_X2;
		points[1].y = TRN_Y2;
		points[2].x = TRN_X3;
		points[2].y = TRN_Y3;*/
	   
		col.vm_color_565 = VM_COLOR_RED;
		col.vm_color_888 = VM_COLOR_565_TO_888(VM_COLOR_RED);

		/* sets global color */
		vm_graphic_setcolor(&col);

		/* get the target buffer*/
		buffer = vm_graphic_get_layer_buffer(layer_hdl[0]);

		//vm_graphic_fill_rect(buffer, MRE_SET_SRC_LEFT_TOP_X, MRE_SET_SRC_LEFT_TOP_Y, vm_graphic_get_screen_width(), 
		//					 vm_graphic_get_screen_height(), VM_COLOR_WHITE, VM_COLOR_GREEN);

		/* Draw 3 points from points on the layer */
		vm_graphic_fill_polygon(layer_hdl[0], points, 3);

		/* Gotta flush */
		//vm_graphic_flush_layer(layer_hdl, 1);
	}

	/*
		Stroke Polygon
		Draws polygon of count points from points as outline
		Called by command: NK_COMMAND_POLYGON
	*/
	static void
	nk_mre_stroke_polygon(const struct nk_vec2i *pnts, int count, 
		unsigned short line_thickness, struct nk_color colr)
	{
			
		VMUINT8 *buffer;
		VMUINT16 color;
		//VMINT TRN_X1, TRN_X2, TRN_X3, TRN_Y1, TRN_Y2, TRN_Y3;

		vm_graphic_color col;
		vm_graphic_point points[3];

		color = convert_color(colr);

		/*TRN_X1 = x0;
		TRN_X2 = x1;
		TRN_X3 = x2;
		TRN_Y1 = y0;
		TRN_Y2 = y1;
		TRN_Y3 = y2;*/

		/* function body */
		vm_log_debug("draw triangle starts");

		/* for every points in vec2i pts put value in points..*/

		/*points[0].x = TRN_X1;
		points[0].y = TRN_Y1;
		points[1].x = TRN_X2;
		points[1].y = TRN_Y2;
		points[2].x = TRN_X3;
		points[2].y = TRN_Y3;*/
	   
		col.vm_color_565 = VM_COLOR_RED;
		col.vm_color_888 = VM_COLOR_565_TO_888(VM_COLOR_RED);

		/* sets global color */
		//vm_graphic_setcolor(&col);

		/* get the target buffer*/
		buffer = vm_graphic_get_layer_buffer(layer_hdl[0]);

		//vm_graphic_fill_rect(buffer, MRE_SET_SRC_LEFT_TOP_X, MRE_SET_SRC_LEFT_TOP_Y, vm_graphic_get_screen_width(), 
		//					 vm_graphic_get_screen_height(), VM_COLOR_WHITE, VM_COLOR_GREEN);

		/* Draw 3 points from points on the layer */
		vm_graphic_fill_polygon(layer_hdl[0], points, 3);

		/* Gotta flush */
		//vm_graphic_flush_layer(layer_hdl, 1);

	}

	/*
		Stroke PolyLine
		Draws a polyline as outline
		Called by command: NK_COMMAND_POLYLINE
	*/
	static void
	nk_mre_stroke_polyline(const struct nk_vec2i *pnts, int count,
		unsigned short line_thickness, struct nk_color col)
	{
		//VMUINT8 *buffer;
		VMUINT16 color;

		color = convert_color(col);

		if (count > 0) {
			int i;
			//MoveToEx(dc, pnts[0].x, pnts[0].y, NULL);
			for (i = 1; i < count; ++i)
			{
				//LineTo(dc, pnts[i].x, pnts[i].y);
				//Replace this with draw line
			}
		}

	}

	/*	
		Fill Circle
		Draws a circle and fills the area inside of it
		Called by command: NK_COMMAND_CIRCLE_FILLED
	*/
	static void
	nk_mre_fill_circle(short x, short y, unsigned short w,
		unsigned short h, struct nk_color col)
	{
		VMUINT8 *buffer;
		VMUINT16 color;
		
		color = convert_color(col);
		buffer = vm_graphic_get_layer_buffer(layer_hdl[0]);

		vm_graphic_ellipse(buffer,x, y, w, h, VM_COLOR_BLUE);

	}

	/*
		Stroke Circle
		Draws a circle's outline
		Called by command: NK_COMMAND_CIRCLE
	*/
	static void
	nk_mre_stroke_circle(short x, short y, unsigned short w,
		unsigned short h, unsigned short line_thickness, struct nk_color col)
	{
		VMUINT8 *buffer;
		VMUINT16 color;
		VMINT REC_X, REC_Y, REC_W, REC_L;

		color = convert_color(col);
		REC_X =	x;
		REC_Y = y;
		REC_W = w;
		REC_L = h;

		/*Get the target buffer */
		buffer = vm_graphic_get_layer_buffer(layer_hdl[0]);

		//vm_graphic_fill_rect(buffer, MRE_SET_SRC_LEFT_TOP_X, MRE_SET_SRC_LEFT_TOP_Y, vm_graphic_get_screen_width(), 
		//                 vm_graphic_get_screen_height(), VM_COLOR_WHITE, VM_COLOR_BLUE);

		//void vm_graphic_ellipse (VMUINT8 * buf, VMINT x, VMINT y, VMINT width, VMINT height, VMUINT16 color);
		/*TODO: Figure this out */
		vm_graphic_ellipse(buffer,x, y, w, h, VM_COLOR_BLUE);

		/* 
			Gotta flush:
			Flush updates the LCD 
		*/
		//vm_graphic_flush_layer(layer_hdl, 1); 

	}

	/*
		Stroke Curve
		Draws a curve outline
		Called by command: NK_COMMAND_CURVE
	*/
	static void
	nk_mre_stroke_curve(struct nk_vec2i p1, struct nk_vec2i p2, 
		struct nk_vec2i p3, struct nk_vec2i p4, unsigned short line_thickness,
			struct nk_color col)
	{
		VMUINT8 *buffer;
		VMUINT16 color;
		//VMINT REC_X, REC_Y, REC_W, REC_L;

		color = convert_color(col);
		
		/*Get the target buffer */
		buffer = vm_graphic_get_layer_buffer(layer_hdl[0]);

		//vm_graphic_fill_rect(buffer, MRE_SET_SRC_LEFT_TOP_X, MRE_SET_SRC_LEFT_TOP_Y, vm_graphic_get_screen_width(), 
		//                 vm_graphic_get_screen_height(), VM_COLOR_WHITE, VM_COLOR_BLUE);

		//void vm_graphic_ellipse (VMUINT8 * buf, VMINT x, VMINT y, VMINT width, VMINT height, VMUINT16 color);
		/*TODO: Figure this out*/
		//vm_graphic_fill_ellipse(buffer,...);

		/* 
			Gotta flush:
			Flush updates the LCD 
		*/
		//vm_graphic_flush_layer(layer_hdl, 1); 

	}

	/*
		Draw Text
		Draws text
		Called by command: NK_COMMAND_TEXT
	*/
	static void
	nk_mre_draw_text(short x, short y, unsigned short w, unsigned short h, 
		const char *text, int len, struct nk_color cbg, struct nk_color col)
	{
		VMUINT8 *buffer;
		VMUINT16 color;
		VMWCHAR display_string[MRE_STR_SIZE_MAX];
	
		color = convert_color(col);
		
		/*Get the target buffer */
		buffer = vm_graphic_get_layer_buffer(layer_hdl[0]);

		/*
			Text it out
			We need to convert strings to usc2 format that 
			MRE only suppports 
			Then we draw the text on the screen
		*/
		/* converts string into usc2 format to display on the screen */
		vm_gb2312_to_ucs2(display_string, MRE_STR_SIZE_MAX, (VMSTR)text);

		/* 
			Draw Text 
			*  disp_buf : [IN] display buffer pointer 
			* x : [IN] x offset of start position
			* y : [IN] y offset of start position
			* s : [IN] string pointer
			* length : [IN] string length
			* color : [IN] string color
		*/
			vm_graphic_textout(buffer, x, 
			y, display_string, 
			wstrlen(display_string), VM_COLOR_WHITE);

		/* 
			Gotta flush:
			Flush updates the LCD 
		*/
		//vm_graphic_flush_layer(layer_hdl, 1);


	}

	/*
		Draw Default Image
		Draws defaut image. Used in testing
		Called by command: NK_COMMAND_IMAGE
	*/
	static void
	nk_mre_draw_image_default(short x, short y, short w, short h)
	{


	}

	__inline clamp(VMUINT16 val, VMUINT16 max) {
		return val < max ? val : max;
	}

	VMUINT16 rotate_hue(
		const VMUINT16 color,
		float H
	)
	{
	  //float U = cos(H*M_PI/180);
	  //float W = sin(H*M_PI/180);
	  float U = nk_cos(H*M_PI/180);
	  float W = nk_sin(H*M_PI/180);


	  VMUINT8 in_r = (color >> (6 + 5)) & 0x1F;
	  VMUINT8 in_g = (color >> 5) & 0x3F;
	  VMUINT8 in_b = color & 0x1F;

	  VMUINT16 r = clamp((.299+.701*U+.168*W)*in_r
		+ (.587-.587*U+.330*W)*in_g
		+ (.114-.114*U-.497*W)*in_b, 31);
	  VMUINT16 g = clamp((.299-.299*U-.328*W)*in_r
		+ (.587+.413*U+.035*W)*in_g
		+ (.114-.114*U+.292*W)*in_b, 63);
	  VMUINT16 b = clamp((.299-.3*U+1.25*W)*in_r
		+ (.587-.588*U-1.05*W)*in_g
		+ (.114+.886*U-.203*W)*in_b, 31);
	  return (r << (6 + 5)) | (g << 5) | b;
	}

	#define CHROMA_KEY 47092

	static void draw_simple_sprite_scaled(
		VMUINT8* sprite_buffer,
		VMINT sprite_x,
		VMINT sprite_y,
		VMINT sprite_w,
		VMINT sprite_h,
		VMINT scale,
		VMUINT8* dest, 
		VMINT x, VMINT y
	) {
		VMINT s_x, s_y; // sprite pixel x, y
		for (s_y = sprite_y; s_y < sprite_y + sprite_h; s_y++) {
			VMINT t_x = x;
			for (s_x = sprite_x + sprite_w * 0; s_x < sprite_x + sprite_w * 0 + sprite_w; s_x++) {
				// Redraw each pixel as a filled rect with scale width & height.
				VMUINT16 pixel = vm_graphic_get_pixel(sprite_buffer, s_x, s_y);
				if (pixel != CHROMA_KEY) {
					vm_graphic_fill_rect(dest, t_x, y, scale, scale, pixel, pixel);
				}
				t_x+=scale;
			}
			y+=scale;
		}
	}

	static void draw_sprite_scaled(VMUINT8* sprite_buffer, struct SpritePtr sprite, VMINT frame, VMINT scale, VMUINT8* dest, VMINT x, VMINT y) {
		if (sprite.type == SPRITE_SIMPLE) {
			struct SpriteSimple const * const ss = sprite.ptr.simple_sprite;
			draw_simple_sprite_scaled(sprite_buffer, ss->x, ss->y, ss->w, ss->h, scale, dest, x, y);
		} else {
			// Packed sprite
			struct SpritePacked const * const ps = sprite.ptr.packed_sprite;
			struct SpritePackedFrame const * packed_frame = &ps->frames[frame];
			struct SpritePart const * sprite_parts = ps->parts;
			VMINT p;
			for (p = 0; p < packed_frame->part_count; p++) {
				struct RenderPart const * rpart = &packed_frame->render_parts[p];
				struct SpritePart const * spart = &sprite_parts[rpart->part_index];
				draw_simple_sprite_scaled(
					sprite_buffer, 
					spart->x,
					spart->y,
					spart->w,
					spart->h,
					scale,
					dest,
					x + rpart->r_x * scale,
					y + rpart->r_y * scale
				);
			}
		}
	}

	/*
		Draw Image
		Draws image set in image object
		Called by command: NK_COMMAND_IMAGE
	*/
	static void
	nk_mre_draw_image(short x, short y, short w, short h, short scale, struct nk_image img)
	{
		// HACKED UP TO ASSUME ALL IMAGES ARE ON SPRITE SHEETS
		VMUINT8 *res_data = NULL;
		VMINT res_size = 0;
		VMINT hcanvas = VM_GDI_SUCCEED;
		VMUINT8 *buffer;

		struct frame_prop * frame_ptr = NULL;

		res_data = vm_load_resource((char*)img.path, &res_size); 
		
		if (res_data == NULL) {
			SPAM(("Failed to load image data\n"));
			nk_mre_draw_text(x,y+h/2,w,h,"Missing image",strlen("Missing image"), nk_black, nk_red);
			return;
		}
 
		if (img.pre_draw) img.pre_draw(res_data);

		hcanvas = vm_graphic_load_image(res_data, res_size);
		vm_free(res_data);
		buffer = vm_graphic_get_layer_buffer(layer_hdl[0]);

		if (hcanvas >= VM_GDI_SUCCEED){ 
			draw_sprite_scaled(vm_graphic_get_canvas_buffer(hcanvas), img.sprite, img.frame, img.scale, buffer, x, y);
			vm_graphic_release_canvas(hcanvas);
		} else {
#ifdef DEBUG
			char error[20];
			sprintf(error, "%d\n", hcanvas);
			nk_mre_draw_text(x,y,0,0,error,strlen(error), nk_black, nk_red);
#endif	
		}
	}


	/*
		Clear It all
		No idea what this is doing
	*/
	static void
	nk_mre_clear(struct nk_color col)
	{
		VMUINT16 color;
		color = convert_color(col);

	}

	/*
		Whats a blit?
		No idea what this is doing
		I have a feeling this copies something from memory to memory
		In this case its copying the worked on device context 
			to the gdi's memory device contesxt
		(Probably not needed in MRE. Most similar is: flushing)
	*/
	static void
	nk_mre_blit(void)
	{
		//Do nothing	
	}

	/********************************************************
						FONT STUFF
	********************************************************/

	/*Create the Font*/
	MreFont*
	nk_mrefont_create(const char *name, int size)
	{
		MreFont *font;
		int obj_size = 1;
		//MreFont *font = (MreFont*)vm_calloc(1, sizeof(MreFont));
		obj_size = sizeof(MreFont);
		font = (MreFont*)vm_calloc(obj_size);
		if (!font){
			return NULL;
		}
		/*Create font params*/
		font->size = size;
		return font;
	}

	/*Get Text Width*/
	static float
	nk_mrefont_get_text_width()
	{
		//Do nothing
		return (float)1;
	}

	/*
		Set Font
		Sets the font required in the platform
		Called at the start of the implementation
	*/
	NK_API void
	nk_mre_set_font(void)
	{
		vm_log_debug("Not setting font - MRE will default it");
	}

	/*
		Delete the font
		Deletes the font set
	*/
	void 
	nk_mrefont_del()
	{
		//Do nothing
	}

	/* Get font float 
		Needed in nk_mre_init method for initialising nk's global font object:
		nk_user_font. We need to give it some value. Since MRE's font isn't
		really complete, we just give it some random float value (1) for now
	*/
	static float
	get_font_width(nk_handle handle, float height, const char *text, int len)
	{
		//Get font width in float
		float size;
		float letter_size = 7;
		size = letter_size * len;
		return (float)size;

	}


	/********************************************************
					INITIALISE DECLARATIONS
	********************************************************/

	/* 
		Initialise the Display stuff - fonts, window, etc
	*/
	struct nk_user_font *font;
	NK_API struct nk_context*
	nk_mre_init(struct nk_user_font *mrefont, unsigned int width, unsigned int height){
	
		mre_view = NULL;
		//Maybe keep some of this outside ? : Not sure. Needs to be thought after its working ok
		
		/* Define window/layer */
		//Layer already initiated in mre_io_sample.h:
		//VMINT		layer_hdl[1];

		/*Declaring the buffer for drawing*/



		/*Declare the nk_user_font*/
		font = mrefont;

		/*Initialising the layer handle*/
		layer_hdl[0] = -1;

		/*Create nk_user_font object from implementations' font object*/
		//struct nk_user_font *font = &mrefont->nk;
		//font->userdata = nk_handle_ptr(mrefont); //Creates an nk_hanlde off that ptr (nk_handle has id and ptr)
		//font->height = (float)mrefont->size;
		//font->width = get_font_width; //Set it to whatever
		
		

		//TODO: Check..

		/*Setting Global data for Graphics(set width, height, font size*/
		//mre_set_graphic_global_data();
		vm_font_set_font_size(VM_SMALL_FONT);
		/*Set language, system, key event callbacks*/
		//vm_mul_lang_set_lang (VM_LANG_ENG);	//Sets Language

		/*
		vm_mul_lang_set_lang (VM_LANG_ENG);
		vm_reg_sysevt_callback (handle_sysevt);
		vm_reg_keyboard_callback (handle_keyevt);
		vm_reg_pen_callback (handle_penevt);
		*/
		
				

		//vm_mul_lang_set_lang (VM_LANG_ENG);


		//Pen callback - not really needed
		//vm_log_file ("declared buffer, initialised layer handle,set global data done.\n");
		/*

		*/
		
		/*Set the mre struct object*/
		mre.width = width;
		mre.height = height;

		/*PAINTING AND PREPARING THE LAYER HANDLER*/ 

		nk_init_mre(&mre.ctx, font);	
		vm_log_debug ("Initialised ALL OK.");

		/*Init Memory Allocations for context*/
		//nk_init_default(&mre.ctx, 0); //Wont work uses StandardLib
		//nk_init_mre(&mre.ctx, 0); //Old before font bug was discovered.
		vm_reg_sysevt_callback (nk_mre_handle_sys_event); //looks at implementation event handler (does thesame thing)
		return &mre.ctx;
	}

	NK_API void
	nk_mre_handle_pen_event(VMINT event, VMINT x, VMINT y) {
		switch (event) {
			case VM_PEN_EVENT_TAP:
				nk_input_motion(&mre.ctx, x, y);
				nk_input_button(&mre.ctx, NK_BUTTON_LEFT, x, y, 1);
				update_gui();
				break;
			case VM_PEN_EVENT_RELEASE:
				nk_input_motion(&mre.ctx, x, y);
				nk_input_button(&mre.ctx, NK_BUTTON_LEFT, x, y, 0);
				nk_input_button(&mre.ctx, NK_BUTTON_RIGHT, x, y, 0);
				update_gui();
				break;
			case VM_PEN_EVENT_LONG_TAP/*VM_PEN_EVENT_DOUBLE_CLICK*/:
				nk_input_motion(&mre.ctx, x, y);
				nk_input_button(&mre.ctx, NK_BUTTON_RIGHT, x, y, 1);
				update_gui();
				nk_input_button(&mre.ctx, NK_BUTTON_RIGHT, x, y, 0);
				break;
			case VM_PEN_EVENT_MOVE:
				nk_input_motion(&mre.ctx, x, y);
				update_gui();
				break;
			default: break;
		}
	}

	/*
		Handle Platform Events
		Platform specific event handler
		Declared at header file
		Initialised at init
	*/
	NK_API void
	nk_mre_handle_key_event(VMINT event, VMINT keycode) {
	
		if (event == VM_KEY_EVENT_UP)
		{
			//int down = VM_KEY_DOWN;
 			int down = VM_KEY_EVENT_UP;
			//int down = (lparam >> 31) & 1;
			//int ctrl = GetKeyState(VK_CONTROL) & (1 << 15);
			//int ctrl = VM_KEY_CTRL;

			switch (keycode) 
			{
				//case VM_KEY_SHIFT:	//Right, Left is all one in MRE
				//	nk_input_key(&mre.ctx, NK_KEY_SHIFT, down);

				//case VM_KEY_DEL:
				//	nk_input_key(&mre.ctx, NK_KEY_DEL, down);

				case VM_KEY_OK: //OK/Enter should do the same, Yeah?
					//nk_input_key(&mre.ctx, NK_KEY_ENTER, down);
				        //nk_input_motion(&mre.ctx, x, y);
				        nk_input_button(&mre.ctx, NK_BUTTON_LEFT, 120, 120, 1);
				        update_gui();
				        break;

				//case VM_KEY_TAB:
				//	nk_input_key(&mre.ctx, NK_KEY_TAB, down);

				case VM_KEY_LEFT:
					//nk_input_key(&mre.ctx, NK_KEY_LEFT, down);
				        update_gui();
				        break;

				case VM_KEY_RIGHT:
					//nk_input_key(&mre.ctx, NK_KEY_RIGHT, down);
				        update_gui();
				        break;

				case VM_KEY_RIGHT_SOFTKEY: //Should be the same right?
				//case VM_KEY_BACK:
					//nk_input_key(&mre.ctx, NK_KEY_BACKSPACE, down);
                                        if (layer_hdl[0] != -1) {
                                        vm_graphic_delete_layer(layer_hdl[0]);
                                        layer_hdl[0] = -1;
                                        }
                                        vm_exit_app();
				        //update_gui();
				        break;

				/*
				case VK_HOME:
					nk_input_key(&gdi.ctx, NK_KEY_TEXT_START, down);
					nk_input_key(&gdi.ctx, NK_KEY_SCROLL_START, down);
					return 1;

				case VK_END:
					nk_input_key(&gdi.ctx, NK_KEY_TEXT_END, down);
					nk_input_key(&gdi.ctx, NK_KEY_SCROLL_END, down);
					return 1;
				*/

				case VM_KEY_DOWN:
					//nk_input_key(&mre.ctx, NK_KEY_SCROLL_DOWN, down);
					//debug info: nk_input_key(ctx, 27, -2);
					//if (mre_view.hoverable_counter < mre_view.hoverable_items){
					//	mre_view.hoverable_counter++;
					//}

					update_gui();
					break;

				case VM_KEY_UP:
					//nk_input_key(&mre.ctx, NK_KEY_SCROLL_UP, down);
					//if (mre_view.hoverable_counter > 0){
					//	mre_view.hoverable_counter--;
					//}else{
					//	mre_view.hoverable_counter = 0;
					//}
					update_gui();
					break;

				//case VM_KEY_POUND:
				//	set_test2_view();

				//	update_gui();
				//	break;
				//case VM_KEY_STAR:
				//	set_test_view();

				//	update_gui();
				//	break;

				//case VM_KEY_C:
				//	if (ctrl) {
				//		nk_input_key(&mre.ctx, NK_KEY_COPY, down);
				//	}
				//	break;

				//case VM_KEY_V:
				//	if (ctrl) {
				//		nk_input_key(&mre.ctx, NK_KEY_PASTE, down);
				//	}
				//	break;

				//case VM_KEY_X:
				//	if (ctrl) {
				//		nk_input_key(&mre.ctx, NK_KEY_CUT, down);
				//	}
				//	break;

				//case VM_KEY_Z:
				//	if (ctrl) {
				//		nk_input_key(&mre.ctx, NK_KEY_TEXT_UNDO, down);
				//	}
				//	break;

				//case VM_KEY_R:
				//	if (ctrl) {
				//		nk_input_key(&mre.ctx, NK_KEY_TEXT_REDO, down);
				//	}
				//	break;
                                default: break;
			}

		}

	}


	/*
		Handles Key Press events
		Declared in header file
		Initialised at init 
	*/
	NK_API void
	nk_mre_handle_sys_event	(VMINT message, VMINT param){
		//nk_mre_handle_event(Event crumb(s)){
		//crumb being values we can judge what event happened..
		VMUINT8 *layer_buf;
		//Do stuff with the events. System Event
		game_sys_event_handler(message, param);
		switch (message) 
		{
			case VM_MSG_CREATE:
				/* the GDI operation is not recommended as the response of the message*/
				break;
			case VM_MSG_ACTIVE:
				//break;
			case VM_MSG_PAINT:
				vm_log_debug ("paint message, support bg");
	            	/* create base layer that has same size as the screen*/
				layer_hdl[0] = vm_graphic_create_layer(MRE_SET_SRC_LEFT_TOP_X, MRE_SET_SRC_LEFT_TOP_Y, 
					vm_graphic_get_screen_width(), 
					vm_graphic_get_screen_height(), VM_NO_TRANSPARENT_COLOR);

				/*
					set clip area
					Remember to reset clip region after drawing
				*/
				vm_graphic_set_clip (0, 0, 
						vm_graphic_get_screen_width (), 
						vm_graphic_get_screen_height ());
				//vm_graphic_set_clip(CLIP_LT_X, CLIP_LT_Y, CLIP_RT_X,  CLIP_RT_Y);

				/* get the layer handler'target buffer*/
				layer_buf = vm_graphic_get_layer_buffer (layer_hdl[0]);
								vm_reg_keyboard_callback (nk_mre_handle_key_event);
				//Keyboard callback
				vm_reg_pen_callback (nk_mre_handle_pen_event);
				
				/* fill the whole screen with BLACK with WHITE as line color*/
				vm_graphic_fill_rect (layer_buf, MRE_SET_SRC_LEFT_TOP_X, MRE_SET_SRC_LEFT_TOP_Y, vm_graphic_get_screen_width (), 
									 vm_graphic_get_screen_height (), VM_COLOR_WHITE, VM_COLOR_RED);
				
	
				/* Flushing - this will actually flush and display on the screen */
				vm_graphic_flush_layer (layer_hdl, 1);
				//layer_hdl[0] = vm_graphic_create_layer (0, 0, vm_graphic_get_screen_width (), 
				//								vm_graphic_get_screen_height (), -1);
				//	vm_graphic_active_layer (layer_hdl[0]);
				/* set clip area */
				//	vm_graphic_set_clip (0, 0, 
				//	vm_graphic_get_screen_width (), 
				//	vm_graphic_get_screen_height ());
	           
				//Starts the main menu guts of the application..
				//mre_start_menu ();
				//initiate_nuklear_gui();
			

			
				//mre_view.components_count = 0;
				update_gui();

				//initiate_nk_gui();
				//draw_hello();
				break;

			case VM_MSG_HIDE:
			case VM_MSG_INACTIVE:
				//printf("TEST INACTIVE");
				vm_log_debug ("VM_MSG_HIDE message, support bg");
				vm_graphic_delete_layer (layer_hdl[0]);
				break;

			case VM_MSG_QUIT:
				vm_log_debug ("VM_MSG_QUIT message, support bg");
				if ( layer_hdl[0] != -1 )
				{
					vm_graphic_delete_layer (layer_hdl[0]);
					layer_hdl[0] = -1;
				}
				break;
		}

		//This is similar to MRE's handle_sysevt(VMINT message, VMINT param).
		// We could call that method from here..
		//handle_keyevt(message, param);
	}

	/*
		Shutdown the MRE display stuff
		Delete from mem and display/bitmap
		Declared during initialisation
	*/
	NK_API void
	nk_mre_shutdown(void){

		/*Flush layer - Like dry the layers together / Refresh*/ 
		vm_graphic_flush_layer(layer_hdl, 2); 

		/* Delete top layer */ 
		//vm_graphic_delete_layer(layer_hdl[1]); 
		//layer_hdl[1] = -1; 
		//layer_buf[1] = NULL; 
		//Resets clip region to full size.
		//vm_graphic_reset_clip();	
	}

	/*
		Render Event handler - Actually draws the UI
		This function loops through all the primitive
		draw calls and calls MRE implementation equivalents.
		
		This function is triggered whenever the GUI needs 
		updating. This Event handler calls the implementation bits
		Declared at the begenning
	*/
	NK_API void
	nk_mre_render(struct nk_color clear){

		const struct nk_command *cmd;
		struct nk_context *ctx = &mre.ctx;
		
		/* 
			get the target buffer from the layer
		*/
		//buffer = vm_graphic_get_layer_buffer(layer_hdl[0]);
		vm_graphic_color start_color = {0x20C9, 0x241A49};
		vm_graphic_color end_color = {0x48C5, 0x491A28};

	/* fill the screen*/
		//vm_graphic_fill_rect (buffer, MRE_SET_SRC_LEFT_TOP_X, MRE_SET_SRC_LEFT_TOP_Y, vm_graphic_get_screen_width (), 
		//					 vm_graphic_get_screen_height (), VM_COLOR_WHITE, VM_COLOR_BLACK);

		/*Test: Clearing*/
		//nk_clear(&mre.ctx); //Clears everything here..
		
		// FOR ADDING SKY
		//vm_graphic_gradient_paint_rect(layer_hdl[0], 0, 0,  vm_graphic_get_screen_width (),  vm_graphic_get_screen_height (), start_color, end_color, VM_GDI_GP_VER);

		/*Actual Render bits*/
		nk_foreach(cmd, &mre.ctx)
		{
			switch (cmd->type) {
			case NK_COMMAND_NOP: break;
			case NK_COMMAND_SCISSOR: {
				const struct nk_command_scissor *s =(const struct nk_command_scissor*)cmd;
				//nk_gdi_scissor(memory_dc, s->x, s->y, s->w, s->h);
				//nk_mre_scissor(s->x, s->y, s->w, s->h);
				nk_mre_scissor();
			} break;
			case NK_COMMAND_LINE: {
				const struct nk_command_line *l = (const struct nk_command_line *)cmd;
				//nk_mre_stroke_line(memory_dc, l->begin.x, l->begin.y, l->end.x,
				//	l->end.y, l->line_thickness, l->color);
				nk_mre_stroke_line(l->begin.x, l->begin.y, l->end.x,
					l->end.y, l->line_thickness, l->color);
			} break;
			case NK_COMMAND_RECT: {
				const struct nk_command_rect *r = (const struct nk_command_rect *)cmd;
				//nk_mre_stroke_rect(memory_dc, r->x, r->y, r->w, r->h,
				//	(unsigned short)r->rounding, r->line_thickness, r->color);
				nk_mre_stroke_rect(r->x, r->y, r->w, r->h,
					(unsigned short)r->rounding, r->line_thickness, r->color);
			} break;
			case NK_COMMAND_RECT_FILLED: {
				const struct nk_command_rect_filled *r = (const struct nk_command_rect_filled *)cmd;
				//nk_mre_fill_rect(memory_dc, r->x, r->y, r->w, r->h,
				////	(unsigned short)r->rounding, r->color);
				if (r->color.a > 0)
				nk_mre_fill_rect(r->x, r->y, r->w, r->h,
					(unsigned short)r->rounding, r->color);
			//	else
	
			} break;
			case NK_COMMAND_CIRCLE: {
				const struct nk_command_circle *c = (const struct nk_command_circle *)cmd;
				//nk_mre_stroke_circle(memory_dc, c->x, c->y, c->w, c->h, c->line_thickness, c->color);
				nk_mre_stroke_circle(c->x, c->y, c->w, c->h, c->line_thickness, c->color);
			} break;
			case NK_COMMAND_CIRCLE_FILLED: {
				const struct nk_command_circle_filled *c = (const struct nk_command_circle_filled *)cmd;
				//nk_mre_fill_circle(memory_dc, c->x, c->y, c->w, c->h, c->color);
				nk_mre_fill_circle(c->x, c->y, c->w, c->h, c->color);
			} break;
			case NK_COMMAND_TRIANGLE: {
				const struct nk_command_triangle*t = (const struct nk_command_triangle*)cmd;
				//nk_mre_stroke_triangle(memory_dc, t->a.x, t->a.y, t->b.x, t->b.y,
				//	t->c.x, t->c.y, t->line_thickness, t->color);
				nk_mre_stroke_triangle( t->a.x, t->a.y, t->b.x, t->b.y,
					t->c.x, t->c.y, t->line_thickness, t->color);
			} break;
			case NK_COMMAND_TRIANGLE_FILLED: {
				const struct nk_command_triangle_filled *t = (const struct nk_command_triangle_filled *)cmd;
				//nk_mre_fill_triangle(memory_dc, t->a.x, t->a.y, t->b.x, t->b.y,
				//	t->c.x, t->c.y, t->color);
				nk_mre_fill_triangle(t->a.x, t->a.y, t->b.x, t->b.y,
					t->c.x, t->c.y, t->color);
			} break;
			case NK_COMMAND_POLYGON: {
				const struct nk_command_polygon *p =(const struct nk_command_polygon*)cmd;
				//nk_mre_stroke_polygon(memory_dc, p->points, p->point_count, p->line_thickness,p->color);
				nk_mre_stroke_polygon(p->points, p->point_count, p->line_thickness,p->color);
			} break;
			case NK_COMMAND_POLYGON_FILLED: {
				const struct nk_command_polygon_filled *p = (const struct nk_command_polygon_filled *)cmd;
				//nk_mre_fill_polygon(memory_dc, p->points, p->point_count, p->color);
				nk_mre_fill_polygon(p->points, p->point_count, p->color);		
			} break;
			case NK_COMMAND_POLYLINE: {
				const struct nk_command_polyline *p = (const struct nk_command_polyline *)cmd;
				//nk_mre_stroke_polyline(memory_dc, p->points, p->point_count, p->line_thickness, p->color);
				nk_mre_stroke_polyline( p->points, p->point_count, p->line_thickness, p->color);
			} break;
			case NK_COMMAND_TEXT: {
				const struct nk_command_text *t = (const struct nk_command_text*)cmd;
				/*nk_mre_draw_text(memory_dc, t->x, t->y, t->w, t->h,
					(const char*)t->string, t->length,
					(GdiFont*)t->font->userdata.ptr,
					t->background, t->foreground);*/
				SPAM(("Text starts at (%d,%d) height: %d, width:%d. \"%s\"  \n",
					t->x,t->y,t->h,t->w,t->string));
				nk_mre_draw_text(t->x, t->y, t->w, t->h,
					(const char*)t->string, t->length,
					t->background, t->foreground);
			} break;
			case NK_COMMAND_CURVE: {
				const struct nk_command_curve *q = (const struct nk_command_curve *)cmd;
				//nk_mre_stroke_curve(memory_dc, q->begin, q->ctrl[0], q->ctrl[1],
				//	q->end, q->line_thickness, q->color);
				nk_mre_stroke_curve(q->begin, q->ctrl[0], q->ctrl[1],
					q->end, q->line_thickness, q->color);
			} break;
			case NK_COMMAND_RECT_MULTI_COLOR:
				break;
			case NK_COMMAND_IMAGE: {
				/* Get the image from the command and image object
					nk_mre_draw_image(nk_image_img, pos_x, pos_y, image_file_path);
				So we can have something like this: 
					nk_mre_draw_image(i->x, i->y, i->w, i->h, i->img.thumb .. );
				*/ 
				const struct nk_command_image *i = (const struct nk_command_image *)cmd;
				SPAM(("Finally, lets draw some pictures!"));
				
				//nk_mre_draw_image_default(i->x, i->y, i->w, i->h); //default testing 

				/* 
				nk_image: 
					struct nk_image {nk_handle handle;unsigned short w,h;unsigned short region[4];};
				nk_handle:
					typedef union {void *ptr; int id;} nk_handle;
				*/ 
				nk_mre_draw_image(i->x + i->xo, i->y + i->yo, i->w, i->h, i->scale, i->img);
				SPAM(("Drew the image. Flushing.."));
			} break;
			case NK_COMMAND_ARC:	break;
			case NK_COMMAND_ARC_FILLED:	break;
			default: break;
			}
			//Testing: Check as it draws..
			//vm_graphic_flush_layer(layer_hdl, 1);
		}

		///*One last flush */
		vm_graphic_flush_layer(layer_hdl, 1); 


		nk_clear(&mre.ctx);
	}

#endif


void nk_mre_set_view(struct nk_context* ctx, struct mre_view_p* view) {
	nk_clear(ctx);
	nk_input_begin(ctx);
	if (mre_view && mre_view->cleanup)	mre_view->cleanup(&mre);
	if (view->setup)					view->setup(&mre);
	mre_view = view;
}

void update_gui(){
	/*Declare context*/
    struct nk_context *ctx;

	VMUINT8 *buffer;

	/* get the target buffer from the layer */
	buffer = vm_graphic_get_layer_buffer(layer_hdl[0]);

	/* get the set context */
	ctx = &mre.ctx;

	SPAM(("\nUpdating the current View..\n"));

	mre_view->view(&mre);
	/*	Draw/Paint - Main render process
		Also set the color to Gray(30,30,30)
	*/
    nk_mre_render(nk_rgb(30,30,30));

	/*	Clears the drawing queue - After drawing
		The queue in context was adding up un necessarily
	*/
	nk_input_begin(ctx); 
}