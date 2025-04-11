#include <openjpeg-2.4/openjpeg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

// --- Callbacks de OpenJPEG ---
void error_callback(const char *msg, void *client_data) {
    (void)client_data;
    fprintf(stderr, "[ERROR] %s\n", msg);
}

void warning_callback(const char *msg, void *client_data) {
    (void)client_data;
    fprintf(stderr, "[ADVERTENCIA] %s\n", msg);
}

void info_callback(const char *msg, void *client_data) {
    (void)client_data;
    fprintf(stdout, "[INFO] %s\n", msg);
}

int main(int argc, char *argv[]) {
    const char* input_filename = "test.jp2";
    const char* output_filename = "output.jpg";

    if (argc > 1) input_filename = argv[1];
    if (argc > 2) output_filename = argv[2];

    // --- Leer archivo JP2 en memoria ---
    FILE* f = fopen(input_filename, "rb");
    if (!f) {
        fprintf(stderr, "No se pudo abrir el archivo: %s\n", input_filename);
        return 1;
    }

    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    fseek(f, 0, SEEK_SET);

    unsigned char* buffer = malloc(size);
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

    // --- Inicializar OpenJPEG ---
    opj_codec_t* codec = opj_create_decompress(OPJ_CODEC_JP2);
    if (!codec) {
        fprintf(stderr, "Error: No se pudo crear el decodificador\n");
        free(buffer);
        return 1;
    }

    opj_set_error_handler(codec, error_callback, NULL);
    opj_set_warning_handler(codec, warning_callback, NULL);
    opj_set_info_handler(codec, info_callback, NULL);

    opj_dparameters_t parameters;
    opj_set_default_decoder_parameters(&parameters);

    if (!opj_setup_decoder(codec, &parameters)) {
        fprintf(stderr, "Error al preparar el decodificador\n");
        opj_destroy_codec(codec);
        free(buffer);
        return 1;
    }

    opj_stream_t* stream = opj_stream_create_default_file_stream(input_filename, OPJ_TRUE);
    if (!stream) {
        fprintf(stderr, "Error al crear el stream\n");
        opj_destroy_codec(codec);
        free(buffer);
        return 1;
    }

    opj_image_t* image = NULL;
    if (!opj_read_header(stream, codec, &image) ||
        !opj_decode(codec, stream, image) ||
        !opj_end_decompress(codec, stream)) {
        fprintf(stderr, "Error al procesar la imagen\n");
        opj_image_destroy(image);
        opj_stream_destroy(stream);
        opj_destroy_codec(codec);
        free(buffer);
        return 1;
    }

    // --- Validación y conversión a RGB ---
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

    for (int i = 1; i < 3; i++) {
        if (image->comps[i].w != width || image->comps[i].h != height) {
            fprintf(stderr, "Error: Las componentes tienen diferentes dimensiones\n");
            opj_image_destroy(image);
            opj_stream_destroy(stream);
            opj_destroy_codec(codec);
            free(buffer);
            return 1;
        }
    }

    unsigned char* rgb = malloc(width * height * 3);
    if (!rgb) {
        fprintf(stderr, "Error: No se pudo asignar memoria para la imagen RGB\n");
        opj_image_destroy(image);
        opj_stream_destroy(stream);
        opj_destroy_codec(codec);
        free(buffer);
        return 1;
    }

    int max_value = (1 << image->comps[0].prec) - 1;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = y * width + x;
            for (int c = 0; c < 3; c++) {
                int value = image->comps[c].data[index];
                if (image->comps[c].sgnd)
                    value += max_value / 2;
                value = (value * 255) / max_value;
                if (value < 0) value = 0;
                if (value > 255) value = 255;
                rgb[index * 3 + c] = (unsigned char)value;
            }
        }
    }

    // --- Guardar como PNG usando stb_image_write ---
    if (!stbi_write_png(output_filename, width, height, 3, rgb, width * 3)) {
        fprintf(stderr, "Error al escribir la imagen PNG\n");
    } else {
        printf("Imagen convertida exitosamente: %s\n", output_filename);
    }

    // --- Limpieza ---
    free(rgb);
    opj_image_destroy(image);
    opj_stream_destroy(stream);
    opj_destroy_codec(codec);
    free(buffer);

    return 0;
}