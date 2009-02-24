#include <stdio.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <gtk/gtk.h>
#include <linux/videodev.h>
#include <sys/mman.h>
#include <string.h>
#include "color.h"

#define CLEAR(x) memset (&(x), 0, sizeof (x))

char dev_name[128] = "/dev/video0";
int width = 320;
int height = 240;
int preview_width = 533;
int preview_height = 400;
int FPS = 15;

struct buffer {
        void *                  start;
        size_t                  length;
};

buffer *buffers = NULL;

int fd = -1;
int reqbuf_count = 4;


GtkWidget *layout;
GtkWidget *window;
GtkWidget *image_face;
static GdkPixmap *pixmap = NULL;

unsigned char framebuffer[2048 * 1536 * 3];

guint timeId = 0;

void open_device();
void init_device();
void set_format();
void request_buffer();
void query_buf_and_mmap();void queue_buffer();
void stream_on();
void read_frame();
static gboolean show_camera(gpointer data);
void stream_off();
void mem_unmap_and_close_dev();
int xioctl(int fd, int request, void* arg);
static gboolean delete_event(GtkWidget *widget,
                             GdkEvent  *event,
                             gpointer   data);


int main( int argc, char *argv[])
{
	gtk_init (&argc, &argv);

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	layout = gtk_layout_new (NULL, NULL);
	gtk_widget_set_size_request(window, 800, 440);
	gtk_layout_set_size((GtkLayout*)layout, 800, 440);

	gtk_window_set_title (GTK_WINDOW (window), "Camera");
	gtk_container_add(GTK_CONTAINER(window), layout);

	g_signal_connect (G_OBJECT (window), "delete_event",
		      G_CALLBACK (delete_event), NULL);

	gtk_container_set_border_width (GTK_CONTAINER (window), 0);

	image_face = gtk_drawing_area_new ();
	gtk_widget_set_size_request(image_face, preview_width, preview_height);
	gtk_layout_put(GTK_LAYOUT(layout), image_face, 135, 0);
	gtk_widget_show_all(window);

	open_device();

   	init_device();

	stream_on();

	// defined in color.h, and used to pixel format changing 
	initLut(); 

	timeId = g_timeout_add(1000/FPS, show_camera, NULL);

	gtk_main ();

	freeLut();

	stream_off();

	mem_unmap_and_close_dev();

	return 0;
}

void open_device()
{
	fd = open (dev_name, O_RDWR|O_NONBLOCK, 0);
	if(-1 == fd) {
		printf("open device error\n");
		/* Error handler */ 
	}
}

void init_device()
{
	set_format();
	request_buffer();
	query_buf_and_mmap();	queue_buffer();
}

void set_format()
{
	struct v4l2_format fmt;
	CLEAR (fmt);

	fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width       = width;
	fmt.fmt.pix.height      = height;
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;

 	if(-1 == xioctl (fd, VIDIOC_S_FMT, &fmt))
 	{
		printf("set format error\n");
   		// Error handler
	}
}

void request_buffer()
{
	struct v4l2_requestbuffers req;
	CLEAR (req);

	req.count               = reqbuf_count;
	req.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory              = V4L2_MEMORY_MMAP;
	if(-1 == xioctl (fd, VIDIOC_REQBUFS, &req))
	{
		printf("request buf error\n");
		// Error handler
	}
}

void query_buf_and_mmap()
{
	buffers = (buffer*)calloc(reqbuf_count, sizeof(*buffers));
	if(!buffers)
	{
		// Error handler
	}

	struct v4l2_buffer buf;
	for(int i = 0; i < reqbuf_count; ++i)
	{
		CLEAR (buf);

		buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index  = i;

		if(-1 == xioctl(fd, VIDIOC_QUERYBUF, &buf))
        {
			printf("query buf error\n");
        	// Error handler
		}
		printf("buffer length: %d\n", buf.length);
		printf("buffer offset: %d\n", buf.m.offset);	

		buffers[i].length = buf.length;
		buffers[i].start = mmap(NULL,
									buf.length,
									PROT_READ|PROT_WRITE,
									MAP_SHARED,
									fd,
									buf.m.offset);

		if(MAP_FAILED == buffers[i].start)
		{
			// Error handler
		}
	}
}

void queue_buffer()
{
	struct v4l2_buffer buf;

	for(int i = 0; i < reqbuf_count; ++i) {
		CLEAR (buf);
		buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;		buf.index  = i;

		if(-1 == xioctl(fd, VIDIOC_QBUF, &buf))
		{
			// Error handler
		}
	}
}

void stream_on()
{
	int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;	if(-1 == xioctl(fd, VIDIOC_STREAMON, &type))
	{
		// Error handler
	}
}

void read_frame()
{	
	struct v4l2_buffer buf;
	unsigned int i;
       
	CLEAR (buf);
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;
	if(-1 == xioctl (fd, VIDIOC_DQBUF, &buf)) 
	{
		// Error handler
	}
//	assert (buf.index < n_buffers0);
   
	memcpy(framebuffer, buffers[buf.index].start, buf.bytesused);	
	if(-1 == xioctl (fd, VIDIOC_QBUF, &buf))
	{		// Error handler
	} 
}

static gboolean show_camera(gpointer data)
{
	read_frame();

	if(pixmap) {
		g_object_unref(pixmap); // ref count minus one
	}

	pixmap = gdk_pixmap_new (image_face->window, preview_width, preview_height, -1);

	unsigned char *buf2 = new unsigned char[width*height*3];

	Pyuv422torgb24((unsigned char*)framebuffer, buf2, width, height);

	GdkPixbuf *rgbBuf = gdk_pixbuf_new_from_data(buf2, GDK_COLORSPACE_RGB, FALSE, 8,
     														width, height, width*3, NULL, NULL);

	if(rgbBuf != NULL)
	{
		GdkPixbuf* buf = gdk_pixbuf_scale_simple(rgbBuf,
															preview_width,
															preview_height,
															GDK_INTERP_BILINEAR);
		gdk_draw_pixbuf(pixmap,
							image_face->style->white_gc,
							buf,
							0, 0, 0, 0,
							preview_width,
							preview_height,
							GDK_RGB_DITHER_NONE,
							0, 0);
		gdk_draw_drawable(image_face->window,
							  image_face->style->white_gc,
							  pixmap,
							  0, 0, 0, 0,
							  preview_width,
							  preview_height);
		g_object_unref(buf);
		g_object_unref(rgbBuf);
	}

	gtk_widget_show(image_face);

	delete [] buf2;
	return TRUE;
}

void stream_off(void)
{
	int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if(-1 == xioctl(fd, VIDIOC_STREAMOFF, &type))
	{
		// Error handler
	}
}

void mem_unmap_and_close_dev()
{
	for(int i = 0; i < reqbuf_count; ++i)
	{
		if(-1 == munmap(buffers[i].start, buffers[i].length))		{
			// Error hanlder
		}
	}

	free(buffers);
	close(fd);
}

int xioctl(int fd, int request, void* arg)
{	int r;
	do r = ioctl (fd, request, arg);	while (-1 == r && EINTR == errno);

	return r;
}

static gboolean delete_event(GtkWidget *widget,
                             GdkEvent  *event,
                             gpointer   data)
{
    g_source_remove(timeId);
    gtk_main_quit ();
    return FALSE;
}

