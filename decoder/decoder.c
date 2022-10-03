#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <semaphore.h>
#include "../common/include/imageChunk.h"
#include "../common/include/constants.h"
#include <stdint.h>
#include <unistd.h>
#include <gtk/gtk.h>

// sudo apt-get install libgtk-3-dev
// gcc decoder.c -o p1 `pkg-config --cflags --libs gtk+-3.0`
static cairo_surface_t *surface = NULL;

#define SHM_CHUNK "/shm_chunk"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"
//*************************************** Data shared between threads ***************************************
int is_configured = 0;
int *px_drawing_array;
//*************************************** Shared mem ***************************************
Statistic_t* statistic_sh_ptr;
ImageChunk_t* chunk_sh_ptr;
Metadata_Table_t* metadata_sh_ptr;
ImgData_t *img_data;

//*************************************** Structures ***************************************
struct DecoderConfig {
    int execution_mode;
    int decryption_key;
    int img_width, img_height, color_channels;
    unsigned char *img_ptr;
    size_t img_size;
    int metadata_id;
};
struct DecoderConfig decod_cfg;

struct Descriptor {
    int encrypted_px;
    int struct_index;
    int px_position;
    int is_read;
    char insertion_time[9];
};

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
    //struct Descriptor desc = desc_array[pos];
    //return desc;
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

void fill_zero_px_array() {
    for(int i=0; i<decod_cfg.img_size; i++) {
        *(px_drawing_array + sizeof(int)*i) = 0;
    }
}

int decrypt_pixel(int pixel, int key) {
    pixel = pixel ^ key;
    return pixel;
}

void enable_ui_update() {
    px_drawing_array = malloc(sizeof(int) * decod_cfg.img_size);
    fill_zero_px_array();
    is_configured = 1;
}

int configure_shared_data() {
    // Crear memoria de estadisticas
    int stat_sh_id = get_id(STATISTIC_PATH, STATISTIC_BLOCK_SIZE);
    statistic_sh_ptr = malloc(sizeof(Statistic_t));
    obtain_shm_pointer(stat_sh_id, statistic_sh_ptr);
    
    /// Obtener valor del chunk
    int chunk_size;
    sem_wait(&(statistic_sh_ptr->semaphore_statistic));
    chunk_size = statistic_sh_ptr-> chunk_size;
    sem_post(&(statistic_sh_ptr->semaphore_statistic));

    // Calcula el tammano del chunk
    size_t chunk_shm_size = sizeof(ImageChunk_t) + chunk_size * sizeof(Node_t);
    // Crea las memorias compartidas
    int chunk_sh_id = get_id(CHUNK_LIST_PATH, chunk_shm_size);
    int metadata_sh_id = get_id(METADATA_PATH, METADATA_BLOCK_SIZE);

    // Obtiene los punteros a memoria compartida
    chunk_sh_ptr = malloc(sizeof(ImageChunk_t));
    obtain_shm_pointer(chunk_sh_id, chunk_sh_ptr);
    metadata_sh_ptr = malloc(sizeof(Metadata_Table_t));
    obtain_shm_pointer(metadata_sh_id, metadata_sh_ptr);
    
    img_data = malloc(sizeof(ImgData_t));

    /// Incrementar numero de instancias
    sem_wait(&(statistic_sh_ptr->semaphore_statistic));
    statistic_sh_ptr -> total_instances += 1;
    sem_post(&(statistic_sh_ptr->semaphore_statistic));

    ///Hace down para reservar el uso de la tabla de metadata
    sem_down(statistic_sh_ptr, metadata_sh_ptr->sem_metadata_table);

    decod_cfg.metadata_id = -1;
    for(int i=0; i<MAX_METADATA_NODES; i++) {
        if(&((metadata_sh_ptr->metadata_array)[i]) != NULL) {
            if(metadata_sh_ptr->metadata_array[i].has_decoder == 0) {
                metadata_sh_ptr->metadata_array[i].has_decoder = 1;
                decod_cfg.img_size = metadata_sh_ptr->metadata_array[i].total_pixeles;
                decod_cfg.img_width = metadata_sh_ptr->metadata_array[i].width;
                decod_cfg.img_height = metadata_sh_ptr->metadata_array[i].height;
                decod_cfg.metadata_id = i;
                // Can start updating UI
                enable_ui_update();
                break;
            }
        }
    }
    if(decod_cfg.metadata_id == -1) {
        printf("No hay imagen con quien enlazarse\n");
        ///Hace up para liberar el uso de la tabla de metadata
        sem_post(&(metadata_sh_ptr->sem_metadata_table));
        return -1;
    }
    ///Hace up para liberar el uso de la tabla de metadata
    sem_post(&(metadata_sh_ptr->sem_metadata_table));
    return 1;
}

static int run_decoder() {
    decod_cfg.img_ptr = malloc(sizeof(decod_cfg.img_size));
    unsigned char *px_iter;
    int px_counter = 0;
    while(px_counter < decod_cfg.img_size) {
        ///Image N semaphore down
        sem_down(statistic_sh_ptr, metadata_sh_ptr->metadata_array[decod_cfg.metadata_id].image_semaphore);

        ///Acces shared chunk
        Node_t * node_i = chunk_sh_ptr->head;
        while(!(node_i->dirtyBit && node_i->metadata_id == decod_cfg.metadata_id)){
            node_i = node_i->next;
        }

        /// Calcular tiempo de escritura
        clock_t sem_start;
        clock_t sem_end;

        sem_start = clock();

        /// Realizar lectura
        struct Descriptor desc;
        desc.encrypted_px = node_i->value;
        desc.px_position = node_i->px_position;
        strncpy(desc.insertion_time, node_i->insertion_time, 9);
        desc.struct_index = node_i->index;
        printf("PRINT ELEGANTE\n");
        node_i->dirtyBit = false;

        sem_end = clock();

        sem_wait(&(statistic_sh_ptr->semaphore_statistic));

        /// Incrementar estadisticas
        statistic_sh_ptr -> total_kernel_time += ((double)(sem_end - sem_start)) / CLOCKS_PER_SEC;

        sem_post(&(statistic_sh_ptr->semaphore_statistic));

        /// Up al semaforo del chunk
        sem_post(&(chunk_sh_ptr->sem_encoders));

        // Manejo de la imagen y pixeles
        //px_iter = desc_cfg.img_ptr + (desc.px_position * 3);        // Altera img_ptr en offset de pos*channels
        *(px_drawing_array + desc.px_position) = desc.encrypted_px;     // para dibujar imagen en array[pos]
            // Obtain RGB pixel values
        //int R = desc.encrypted_px & 0xFF0000;
        //int G = desc.encrypted_px & 0x00FF00;
        //int B = desc.encrypted_px & 0x0000FF;
            // Set final image pixel channel values
        //*px_iter = R;
        //*(px_iter + 1) = G;
        //*(px_iter + 2) = B;
        px_counter += 1;
    }

    //********* Decoding image
    for(int i=0; i<decod_cfg.img_size; i++) {
        int hex_color = *(px_drawing_array + i);
        int decrypted_px = decrypt_pixel(hex_color, decod_cfg.decryption_key);
        *(px_drawing_array + i) = decrypted_px;

        // stb_image ptr
        int R = decrypted_px & 0xFF0000;
        int G = decrypted_px & 0x00FF00;
        int B = decrypted_px & 0x0000FF;
        px_iter = decod_cfg.img_ptr + i*3;
        *px_iter = R;
        *(px_iter + 1) = G;
        *(px_iter + 2) = B;
    }

    /// Decrementar numero de instancias
    sem_wait(&(statistic_sh_ptr->semaphore_statistic));
    statistic_sh_ptr -> total_instances -= 1;
    if(statistic_sh_ptr -> total_instances == 0) {
        sem_post(&(statistic_sh_ptr -> semaphore_visualizer));
    }
    sem_post(&(statistic_sh_ptr->semaphore_statistic));
}

//*************************************** Application Flow ***************************************

gboolean update_image(GtkWidget *widget) {
    if(is_configured == 1) {
        for(int i=0; i<decod_cfg.img_size; i++) {
            int hex_color = *(px_drawing_array + i);
            int pos_x = i % decod_cfg.img_width;
            int pos_y = i / decod_cfg.img_height;
            draw_pixel(widget, pos_x, pos_y, hex_color);
        }
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
    get_decoder_params();
    configure_shared_data();

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
    gtk_widget_set_size_request (drawing_area, decod_cfg.img_width, decod_cfg.img_height);
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