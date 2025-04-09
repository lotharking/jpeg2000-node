#include <openjpeg-2.4/openjpeg.h>
#include <jpeglib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Función para escribir imagen como JPEG usando libjpeg
int write_jpeg(const char* filename, unsigned char* data, int width, int height) {
    FILE* outfile = fopen(filename, "wb");
    if (!outfile) {
        perror("No se pudo abrir el archivo de salida");
        return 0;
    }
    
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    JSAMPROW row_pointer[1];
    
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    jpeg_stdio_dest(&cinfo, outfile);
    
    cinfo.image_width = width;
    cinfo.image_height = height;
    cinfo.input_components = 3; // RGB
    cinfo.in_color_space = JCS_RGB;
    
    jpeg_set_defaults(&cinfo);
    jpeg_start_compress(&cinfo, TRUE);
    
    while (cinfo.next_scanline < cinfo.image_height) {
        row_pointer[0] = &data[cinfo.next_scanline * width * 3];
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }
    
    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);
    fclose(outfile);
    return 1;
}

// Callback para el manejo de errores de OpenJPEG
void error_callback(const char *msg, void *client_data) {
    (void)client_data;
    fprintf(stderr, "[ERROR] %s\n", msg);
}

// Callback para el manejo de advertencias de OpenJPEG
void warning_callback(const char *msg, void *client_data) {
    (void)client_data;
    fprintf(stderr, "[ADVERTENCIA] %s\n", msg);
}

// Callback para el manejo de información de OpenJPEG
void info_callback(const char *msg, void *client_data) {
    (void)client_data;
    fprintf(stdout, "[INFO] %s\n", msg);
}

// Función para crear un stream de memoria para OpenJPEG
opj_stream_t* create_memory_stream(unsigned char* data, size_t size) {
    opj_stream_t* stream = opj_stream_default_create(OPJ_TRUE);
    if (stream == NULL) {
        return NULL;
    }
    
    // Configurar funciones de lectura/escritura para trabajar desde memoria
    opj_stream_set_user_data(stream, data, NULL);
    opj_stream_set_user_data_length(stream, size);
    
    return stream;
}

int main(int argc, char *argv[]) {
    const char* input_filename = "test.jp2";
    const char* output_filename = "output.jpg";
    
    // Permitir especificar nombres de archivo a través de argumentos
    if (argc > 1) input_filename = argv[1];
    if (argc > 2) output_filename = argv[2];
    
    // Abrir archivo JP2
    FILE* f = fopen(input_filename, "rb");
    if (!f) {
        fprintf(stderr, "No se pudo abrir el archivo: %s\n", input_filename);
        return 1;
    }
    
    // Obtener tamaño del archivo
    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    // Leer archivo en memoria
    unsigned char* buffer = (unsigned char*)malloc(size);
    if (!buffer) {
        fprintf(stderr, "Error: No se pudo asignar memoria para el buffer\n");
        fclose(f);
        return 1;
    }
    
    size_t read_bytes = fread(buffer, 1, size, f);
    fclose(f);
    
    if (read_bytes != size) {
        fprintf(stderr, "Error: No se pudo leer el archivo completo\n");
        free(buffer);
        return 1;
    }
    
    // Configurar el manejador de OpenJPEG
    opj_codec_t* codec = opj_create_decompress(OPJ_CODEC_JP2);
    if (!codec) {
        fprintf(stderr, "Error: No se pudo crear el decodificador\n");
        free(buffer);
        return 1;
    }
    
    // Configurar callbacks para manejo de errores
    opj_set_error_handler(codec, error_callback, NULL);
    opj_set_warning_handler(codec, warning_callback, NULL);
    opj_set_info_handler(codec, info_callback, NULL);
    
    // Configurar parámetros de decodificación
    opj_dparameters_t parameters;
    opj_set_default_decoder_parameters(&parameters);
    
    if (!opj_setup_decoder(codec, &parameters)) {
        fprintf(stderr, "Error al preparar el decodificador\n");
        opj_destroy_codec(codec);
        free(buffer);
        return 1;
    }
    
    // Crear un stream desde el archivo en disco (alternativa a memory stream)
    opj_stream_t* stream = opj_stream_create_default_file_stream(input_filename, OPJ_TRUE);
    if (!stream) {
        fprintf(stderr, "Error al crear el stream\n");
        opj_destroy_codec(codec);
        free(buffer);
        return 1;
    }
    
    // Leer encabezado y decodificar imagen
    opj_image_t* image = NULL;
    if (!opj_read_header(stream, codec, &image)) {
        fprintf(stderr, "Error al leer encabezado\n");
        opj_stream_destroy(stream);
        opj_destroy_codec(codec);
        free(buffer);
        return 1;
    }
    
    if (!opj_decode(codec, stream, image)) {
        fprintf(stderr, "Error al decodificar imagen\n");
        opj_image_destroy(image);
        opj_stream_destroy(stream);
        opj_destroy_codec(codec);
        free(buffer);
        return 1;
    }
    
    if (!opj_end_decompress(codec, stream)) {
        fprintf(stderr, "Error al finalizar la decodificación\n");
        opj_image_destroy(image);
        opj_stream_destroy(stream);
        opj_destroy_codec(codec);
        free(buffer);
        return 1;
    }
    
    // Verificar componentes de la imagen
    if (image->numcomps < 3) {
        fprintf(stderr, "Error: La imagen debe tener al menos 3 componentes (RGB)\n");
        opj_image_destroy(image);
        opj_stream_destroy(stream);
        opj_destroy_codec(codec);
        free(buffer);
        return 1;
    }
    
    int width = image->comps[0].w;
    int height = image->comps[0].h;
    
    // Verificar que todas las componentes tengan el mismo tamaño
    for (int i = 1; i < 3; i++) {
        if (image->comps[i].w != width || image->comps[i].h != height) {
            fprintf(stderr, "Error: Las componentes de la imagen tienen diferentes dimensiones\n");
            opj_image_destroy(image);
            opj_stream_destroy(stream);
            opj_destroy_codec(codec);
            free(buffer);
            return 1;
        }
    }
    
    // Convertir a RGB plano
    unsigned char* rgb = (unsigned char*)malloc(width * height * 3);
    if (!rgb) {
        fprintf(stderr, "Error: No se pudo asignar memoria para la imagen RGB\n");
        opj_image_destroy(image);
        opj_stream_destroy(stream);
        opj_destroy_codec(codec);
        free(buffer);
        return 1;
    }
    
    // Normalizar los valores según la precisión
    int max_value = (1 << image->comps[0].prec) - 1;
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = y * width + x;
            
            // Normalizar y ajustar los valores a rango 0-255
            for (int c = 0; c < 3; c++) {
                int value = image->comps[c].data[index];
                
                // Manejar valores negativos y precisión
                if (image->comps[c].sgnd) {
                    value += max_value / 2;
                }
                
                // Normalizar al rango 0-255
                value = (value * 255) / max_value;
                
                // Asegurar que esté dentro del rango
                if (value < 0) value = 0;
                if (value > 255) value = 255;
                
                rgb[(y * width + x) * 3 + c] = (unsigned char)value;
            }
        }
    }
    
    // Escribir el archivo JPEG
    if (!write_jpeg(output_filename, rgb, width, height)) {
        fprintf(stderr, "Error al escribir el archivo JPEG\n");
    } else {
        printf("Imagen convertida exitosamente a %s\n", output_filename);
    }
    
    // Limpieza
    free(rgb);
    opj_image_destroy(image);
    opj_stream_destroy(stream);
    opj_destroy_codec(codec);
    free(buffer);
    
    return 0;
}