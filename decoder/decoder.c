#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <gtk/gtk.h>

// sudo apt-get install libgtk-3-dev
// gcc decoder.c -o p1 `pkg-config --cflags --libs gtk+-3.0`
static cairo_surface_t *surface = NULL;

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"

int common_array[500] = { 0 };

//*************************************** Structures ***************************************
struct DecoderConfig {
    int execution_mode;
    int decryption_key;
    int img_width, img_height, color_channels;
    unsigned char *img_ptr;
    size_t img_size;
};
struct DecoderConfig decod_cfg;

struct Descriptor {
    int encrypted_px;
    int struct_index;
    int px_position;
    int is_read;
    char insertion_time[9];
};
struct Descriptor desc_array[2500];

//*************************************** Graphics functions ***************************************
// Obtained from: http://www.freemedialab.org/wiki/doku.php?id=programmazione:gtk:gtk_e_i_thread
static void clear_surface() {
    cairo_t *cr;
    cr = cairo_create(surface);
    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_paint(cr);
    cairo_destroy(cr);
}

static gboolean configure_event_cb (GtkWidget *widget) {
    if(surface)
        cairo_surface_destroy(surface);
    surface = gdk_window_create_similar_surface(gtk_widget_get_window (widget),
                                               CAIRO_CONTENT_COLOR,
                                               gtk_widget_get_allocated_width (widget),
                                               gtk_widget_get_allocated_height (widget));
    /* Initialize the surface to white */
    clear_surface();
    return TRUE;
}

static gboolean redraw_screen (GtkWidget *widget, cairo_t *cr) {
    cairo_set_source_surface(cr, surface, 0, 0);
    cairo_paint(cr);
    return FALSE;
}

static void draw_pixel(GtkWidget *widget, gdouble x, gdouble y, int color) {
    cairo_t *cr;
    cr = cairo_create(surface);
    cairo_set_source_rgb(cr, (color & 0xFF0000), (color & 0x00FF00), (color & 0x0000FF));
    cairo_rectangle(cr, x, y, 1, 1);
    cairo_fill(cr);
    cairo_destroy(cr);
    gtk_widget_queue_draw_area (widget, x, y, 1, 1);    // Invalidate affected region of the drawing area
}

static void close_window (void) {
    if (surface)
      cairo_surface_destroy (surface);
}


//*************************************** Decoder functions ***************************************
struct Descriptor fetch_descriptor(int pos) {       // TODO: connect function to shared memory
    struct Descriptor desc = desc_array[pos];
    return desc;
}

void get_decoder_params() {
    printf("Specify execution mode 0 for auto, 1 for manual: \n");
    scanf("%d", &decod_cfg.execution_mode);

    printf("Specify encryption key: \n");
    scanf("%d", &decod_cfg.decryption_key);
}

int get_image_data() {      // TODO: fetch image size from meta data
    decod_cfg.img_height = 600;
    decod_cfg.img_width = 600;
    decod_cfg.img_size = decod_cfg.img_height * decod_cfg.img_width;
}

int decrypt_pixel(int pixel, int key) {
    pixel = pixel ^ key;
    return pixel;
}

static int run_decoder() {
    decod_cfg.img_ptr = malloc(sizeof(decod_cfg.img_size));
    // for testing GTK      TODO: remove for
    for (int i=0; i<500; i++) {
        common_array[i] = 1;
        sleep(1);
    }

    unsigned char *px_iter;
    int px_counter = 0;
    int is_run = 1;
    /*while(px_counter < decod_cfg.img_size) {
        if (is_run == 1) {
            struct Descriptor desc = fetch_descriptor(px_counter);
            px_iter = img_ptr + desc.px_position;
            // Obtain RGB pixel values
            int R = desc.encrypted_px & 0xFF0000;
            int G = desc.encrypted_px & 0x00FF00;
            int B = desc.encrypted_px & 0x0000FF;
            //putpixel(px_counter%decod_cfg.img_size, px_counter/decod_cfg.img_size, desc.encrypted_px);        TODO: draw pixel
            // Insert px RGB in image file
            *px_iter = R;
            *(px_iter + 1) = G;
            *(px_iter + 2) = B;
            px_counter += 1;
        }
        else { sleep(0.7); }
        is_run *= -1;
    }

    // Decoding
    px_counter = 0;
    for (px_iter = img_ptr; px_iter!=(img_ptr+decod_cfg.img_size); px_iter+=3) {
        //int decrypted_px = decrypt_pixel(px_array[i], decryption_key);
        int R = decrypt_pixel(*px_iter, (*decryption_key & 0xFF0000));
        int G = decrypt_pixel(*(px_iter+1), (*decryption_key & 0x00FF00));
        int B = decrypt_pixel(*(px_iter+2), (*decryption_key & 0x0000FF));
        *px_iter = R;
        *(px_iter + 1) = G;
        *(px_iter + 2) = B;
        int decrypted_px = (R << 16) + (G << 8) + B;
        //putpixel(px_counter%decod_cfg.img_size, px_counter/decod_cfg.img_size, decrypted_px);     TODO: draw pixel
        px_counter += 1;
    }*/
    return 1;
    free(decod_cfg.img_ptr);
}

//*************************************** Application Flow ***************************************

gboolean update_image(GtkWidget *widget) {
    for (int i=0; i<500; i++) {
        if(common_array[i] == 1)
            draw_pixel(widget, i, i, 0xFF0000);
    }
    return TRUE;
}

void start_decoder_thread(GtkWidget *widget) {
    g_timeout_add(1000, update_image, widget);
    GThread   *decod_thread;
	decod_thread = g_thread_new(NULL,(GThreadFunc)run_decoder, NULL);
	gtk_widget_set_sensitive(widget, FALSE) ;
}

static void init_application (GtkApplication *app, gpointer user_data) {
    // Decoder configuration
    get_image_data();
    get_decoder_params();

    GtkWidget *window;
    GtkWidget *frame;
    GtkWidget *drawing_area;

    window = gtk_application_window_new (app);
    gtk_window_set_title (GTK_WINDOW (window), "Decoded image");
    gtk_container_set_border_width (GTK_CONTAINER (window), 1);
    frame = gtk_frame_new (NULL);
    gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_IN);
    gtk_container_add (GTK_CONTAINER (window), frame);
    drawing_area = gtk_drawing_area_new ();
    
    // Setting size
    gtk_widget_set_size_request (drawing_area, decod_cfg.img_height, decod_cfg.img_width);
    gtk_container_add (GTK_CONTAINER (frame), drawing_area);

    // SIGNALS
    g_signal_connect(window, "destroy", G_CALLBACK (close_window), NULL);
    g_signal_connect(drawing_area,"configure-event", G_CALLBACK (configure_event_cb), NULL);
    g_signal_connect(drawing_area, "draw", G_CALLBACK (redraw_screen), NULL);

    // Execution
    gtk_widget_show_all (window);
    start_decoder_thread(drawing_area);
}

int main(int argc, char **argv) {
    // GTK configuration
    int app_status;
    GtkApplication *app = gtk_application_new("org.gtk.decoded_img", G_APPLICATION_FLAGS_NONE);;
    g_signal_connect(app, "activate", G_CALLBACK (init_application), NULL);
    app_status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);
    return app_status;
}