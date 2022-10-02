#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <gtk/gtk.h>

// sudo apt-get install libgtk-3-dev
// gcc decoder.c -o p1 `pkg-config --cflags --libs gtk+-3.0`
static cairo_surface_t *surface = NULL;

//#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"

//***************************************Graphics functions***************************************
static void clear_surface (void) {
  cairo_t *cr;
  cr = cairo_create (surface);
  cairo_set_source_rgb (cr, 1, 1, 1);
  cairo_paint (cr);
  cairo_destroy (cr);
}

static gboolean configure_event_cb (GtkWidget *widget, GdkEventConfigure *event, gpointer data) {
  if (surface)
    cairo_surface_destroy (surface);

  surface = gdk_window_create_similar_surface (gtk_widget_get_window (widget),
                                               CAIRO_CONTENT_COLOR,
                                               gtk_widget_get_allocated_width (widget),
                                               gtk_widget_get_allocated_height (widget));
  /* Initialize the surface to white */
  clear_surface ();
  /* We've handled the configure event, no need for further processing. */
  return TRUE;
}

/* Redraw the screen from the surface. Note that the ::draw
 * signal receives a ready-to-be-used cairo_t that is already
 * clipped to only draw the exposed areas of the widget
 */
static gboolean draw_cb (GtkWidget *widget, cairo_t *cr, gpointer   data) {
  cairo_set_source_surface (cr, surface, 0, 0);
  cairo_paint (cr);
  return FALSE;
}

/* Draw a rectangle on the surface at the given position */
static void draw_brush (GtkWidget *widget, gdouble x, gdouble y) {
  cairo_t *cr;
  /* Paint to the surface, where we store our state */
  cr = cairo_create (surface);
  cairo_rectangle (cr, x - 3, y - 3, 6, 6);
  cairo_fill (cr);
  cairo_destroy (cr);
  /* Now invalidate the affected region of the drawing area. */
  gtk_widget_queue_draw_area (widget, x - 3, y - 3, 6, 6);
}

/* Handle button press events by either drawing a rectangle
 * or clearing the surface, depending on which button was pressed.
 * The ::button-press signal handler receives a GdkEventButton
 * struct which contains this information.
 */
static gboolean button_press_event_cb (GtkWidget *widget, GdkEventButton *event, gpointer data) {
  /* paranoia check, in case we haven't gotten a configure event */
    if (surface == NULL)
        return FALSE;
    if (event->button == GDK_BUTTON_PRIMARY) {
        draw_brush (widget, event->x, event->y);
    }
    else if (event->button == GDK_BUTTON_SECONDARY) {
      clear_surface ();
      gtk_widget_queue_draw (widget);
    }
  /* We've handled the event, stop processing */
  return TRUE;
}

/* Handle motion events by continuing to draw if button 1 is
 * still held down. The ::motion-notify signal handler receives
 * a GdkEventMotion struct which contains this information.
 */
static gboolean motion_notify_event_cb (GtkWidget *widget, GdkEventMotion *event, gpointer data) {
  /* paranoia check, in case we haven't gotten a configure event */
  if (surface == NULL)
    return FALSE;
  if (event->state & GDK_BUTTON1_MASK)
    draw_brush (widget, event->x, event->y);
  /* We've handled it, stop processing */
  return TRUE;
}

static void close_window (void) {
  if (surface)
    cairo_surface_destroy (surface);
}

static void activate (GtkApplication *app, gpointer user_data) {
  GtkWidget *window;
  GtkWidget *frame;
  GtkWidget *drawing_area;

  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "Drawing Area");

  g_signal_connect (window, "destroy", G_CALLBACK (close_window), NULL);

  gtk_container_set_border_width (GTK_CONTAINER (window), 8);

  frame = gtk_frame_new (NULL);
  gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_IN);
  gtk_container_add (GTK_CONTAINER (window), frame);

  drawing_area = gtk_drawing_area_new ();
  /* set a minimum size */
  gtk_widget_set_size_request (drawing_area, 100, 100);

  gtk_container_add (GTK_CONTAINER (frame), drawing_area);

  /* Signals used to handle the backing surface */
  g_signal_connect (drawing_area, "draw",
                    G_CALLBACK (draw_cb), NULL);
  g_signal_connect (drawing_area,"configure-event",
                    G_CALLBACK (configure_event_cb), NULL);

  /* Event signals */
  g_signal_connect (drawing_area, "motion-notify-event",
                    G_CALLBACK (motion_notify_event_cb), NULL);
  g_signal_connect (drawing_area, "button-press-event",
                    G_CALLBACK (button_press_event_cb), NULL);

  /* Ask to receive events the drawing area doesn't normally
   * subscribe to. In particular, we need to ask for the
   * button press and motion notify events that want to handle.
   */
  gtk_widget_set_events (drawing_area, gtk_widget_get_events (drawing_area)
                                     | GDK_BUTTON_PRESS_MASK
                                     | GDK_POINTER_MOTION_MASK);
  gtk_widget_show_all (window);
}



//***************************************Decoder functions***************************************

struct Descriptor {
    int encrypted_px;
    int struct_index;
    int px_position;
    int is_read;
    char insertion_time[9];
};
struct Descriptor desc_array[2500];

struct Descriptor fetch_descriptor(int pos) {       // TODO: connect function to shared memory
    struct Descriptor desc = desc_array[pos];
    return desc;
}

void get_decoder_params(int *mode, int *key) {
    printf("Specify execution mode 0 for auto, 1 for manual: \n");
    scanf("%d", mode);

    printf("Specify encryption key: \n");
    scanf("%d", key);
}

int get_image_size() {      // TODO: fetch image size from meta data
    return 2500;
}

int decrypt_pixel(int pixel, int key) {
    pixel = pixel ^ key;
    return pixel;
}

int main(int argc, char **argv) {
    // Setting configuration
    /*int IMAGE_SIZE = get_image_size();
    int *execution_mode = malloc(sizeof(int));
    int *decryption_key = malloc(sizeof(int));
    get_decoder_params(execution_mode, decryption_key);*/

    // GTK configuration
    GtkApplication *app;
    int status;
    app = gtk_application_new ("org.gtk.decoded_img", G_APPLICATION_FLAGS_NONE);
    g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
    status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);
    printf("SALGO DE AQUI!");
    //gtk_init(&argc, &argv);
/*
    unsigned char *img_ptr = malloc(IMAGE_SIZE);
    unsigned char *px_iter;
    int px_counter = 0;
    int is_run = 1;
    while(px_counter < IMAGE_SIZE) {
        if (is_run == 1) {
            struct Descriptor desc = fetch_descriptor(px_counter);
            px_iter = img_ptr + desc.px_position;
            // Obtain RGB pixel values
            int R = desc.encrypted_px & 0xFF0000;
            int G = desc.encrypted_px & 0x00FF00;
            int B = desc.encrypted_px & 0x0000FF;
            //putpixel(px_counter%IMAGE_SIZE, px_counter/IMAGE_SIZE, desc.encrypted_px);        TODO: draw pixel
            // Insert px RGB in image file
            *px_iter = R;
            *(px_iter + 1) = G;
            *(px_iter + 2) = B;
            px_counter += 1;
        }
        else { sleep(0.7); }
        is_run *= -1;
    }
    sleep(3);

    // Decoding
    px_counter = 0;
    for (px_iter = img_ptr; px_iter!=(img_ptr+IMAGE_SIZE); px_iter+=3) {
        //int decrypted_px = decrypt_pixel(px_array[i], decryption_key);
        int R = decrypt_pixel(*px_iter, (*decryption_key & 0xFF0000));
        int G = decrypt_pixel(*(px_iter+1), (*decryption_key & 0x00FF00));
        int B = decrypt_pixel(*(px_iter+2), (*decryption_key & 0x0000FF));
        *px_iter = R;
        *(px_iter + 1) = G;
        *(px_iter + 2) = B;
        int decrypted_px = (R << 16) + (G << 8) + B;
        //putpixel(px_counter%IMAGE_SIZE, px_counter/IMAGE_SIZE, decrypted_px);     TODO: draw pixel
        px_counter += 1;
    }
    sleep(10);*/
    return status;
}