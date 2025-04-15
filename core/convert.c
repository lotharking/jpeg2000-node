#include <openjpeg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define TEMP_INPUT "temp_input.jp2"
#define TEMP_OUTPUT "temp_output.bin"

// --- Callbacks ---
void error_callback(const char *msg, void *client_data) { (void)client_data; fprintf(stderr, "[ERROR] %s\n", msg); }
void warning_callback(const char *msg, void *client_data) { (void)client_data; fprintf(stderr, "[WARNING] %s\n", msg); }
void info_callback(const char *msg, void *client_data) { (void)client_data; fprintf(stderr, "[INFO] %s\n", msg); }

static void write_to_file(void *context, void *data, int size) {
    FILE *file = (FILE*)context;
    fwrite(data, 1, size, file);
    fflush(file);
}

int write_temp_input() {
    FILE* f = fopen(TEMP_INPUT, "wb");
    if (!f) return -1;

    unsigned char buf[8192];
    size_t total = 0, read;
    while ((read = fread(buf, 1, sizeof(buf), stdin)) > 0) {
        fwrite(buf, 1, read, f);
        total += read;
    }

    fclose(f);
    if (total == 0) return -2;

    fprintf(stderr, "Read %zu bytes from stdin\n", total);
    return 0;
}

int is_format_supported(const char* fmt) {
    return strcmp(fmt, "png") == 0 || strcmp(fmt, "bmp") == 0 || strcmp(fmt, "jpg") == 0 || strcmp(fmt, "jpeg") == 0;
}

void cleanup_all(opj_image_t* img, opj_codec_t* codec, const char* reason) {
    if (reason) fprintf(stderr, "Error: %s\n", reason);
    if (img) opj_image_destroy(img);
    if (codec) opj_destroy_codec(codec);
    remove(TEMP_INPUT);
    remove(TEMP_OUTPUT);
}

int run_test() {
    fprintf(stderr, "Running test...\n");
    int result = system("cat test.jp2 | ./convert jpg > output.jpg");

    if (result != 0) {
        fprintf(stderr, "Test failed with code %d\n", result);
        return 1;
    }

    FILE* f = fopen("output.jpg", "rb");
    if (!f) {
        fprintf(stderr, "Test failed: output.jpg not created\n");
        return 1;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fclose(f);

    if (size <= 0) {
        fprintf(stderr, "Test failed: output.jpg is empty\n");
        return 1;
    }

    fprintf(stderr, "Test passed: output.jpg created (%ld bytes)\n", size);
    return 0;
}

int main(int argc, char *argv[]) {
    // Run test mode
    if (argc > 1 && strcmp(argv[1], "--test") == 0) {
        return run_test();
    }
    
    const char* format = (argc > 1) ? argv[1] : "png";
    if (!is_format_supported(format)) {
        fprintf(stderr, "Unsupported format: %s\nSupported: png, bmp, jpg, jpeg\n", format);
        return 1;
    }

    int input_status = write_temp_input();
    if (input_status != 0) {
        fprintf(stderr, input_status == -1 ? "Could not create temp input\n" : "No data from stdin\n");
        return 1;
    }

    opj_codec_t* codec = opj_create_decompress(OPJ_CODEC_JP2);
    if (!codec) {
        remove(TEMP_INPUT);
        fprintf(stderr, "Could not create decoder\n");
        return 1;
    }

    opj_set_error_handler(codec, error_callback, NULL);
    opj_set_warning_handler(codec, warning_callback, NULL);
    
    #ifdef DEBUG
    opj_set_info_handler(codec, info_callback, NULL);
    #endif

    opj_dparameters_t params;
    opj_set_default_decoder_parameters(&params);

    if (!opj_setup_decoder(codec, &params)) {
        cleanup_all(NULL, codec, "setup decoder failed");
        return 1;
    }

    opj_stream_t* stream = opj_stream_create_default_file_stream(TEMP_INPUT, OPJ_TRUE);
    if (!stream) {
        cleanup_all(NULL, codec, "stream creation failed");
        return 1;
    }

    opj_image_t* image = NULL;
    if (!opj_read_header(stream, codec, &image) || !opj_decode(codec, stream, image) || !opj_end_decompress(codec, stream)) {
        opj_stream_destroy(stream);
        cleanup_all(image, codec, "decoding failed");
        return 1;
    }

    opj_stream_destroy(stream);

    if (image->numcomps < 3) {
        cleanup_all(image, codec, "less than 3 components (RGB required)");
        return 1;
    }

    int width = image->comps[0].w, height = image->comps[0].h;
    for (int i = 1; i < 3; ++i)
        if (image->comps[i].w != width || image->comps[i].h != height) {
            cleanup_all(image, codec, "component size mismatch");
            return 1;
        }

    unsigned char* rgb = malloc(width * height * 3);
    if (!rgb) {
        cleanup_all(image, codec, "memory allocation failed");
        return 1;
    }

    int max_val = (1 << image->comps[0].prec) - 1;
    for (int i = 0; i < width * height; ++i) {
        for (int c = 0; c < 3; ++c) {
            int val = image->comps[c].data[i];
            if (image->comps[c].sgnd) val += max_val / 2;
            val = (val * 255) / max_val;
            rgb[i * 3 + c] = (unsigned char)(val < 0 ? 0 : val > 255 ? 255 : val);
        }
    }

    FILE* fout = fopen(TEMP_OUTPUT, "wb");
    if (!fout) {
        free(rgb);
        cleanup_all(image, codec, "could not create temp file output");
        return 1;
    }

    int success = 0;
    if (strcmp(format, "png") == 0)
        success = stbi_write_png_to_func(write_to_file, fout, width, height, 3, rgb, width * 3);
    else if (strcmp(format, "bmp") == 0)
        success = stbi_write_bmp_to_func(write_to_file, fout, width, height, 3, rgb);
    else
        success = stbi_write_jpg_to_func(write_to_file, fout, width, height, 3, rgb, 90);

    fclose(fout);
    free(rgb);
    opj_image_destroy(image);
    opj_destroy_codec(codec);

    if (!success) {
        remove(TEMP_INPUT); remove(TEMP_OUTPUT);
        fprintf(stderr, "image write failed\n");
        return 1;
    }

    FILE* out = fopen(TEMP_OUTPUT, "rb");
    if (!out) {
        remove(TEMP_INPUT); remove(TEMP_OUTPUT);
        fprintf(stderr, "could not open output file\n");
        return 1;
    }

    unsigned char buf[8192];
    size_t r;
    while ((r = fread(buf, 1, sizeof(buf), out)) > 0) {
        if (fwrite(buf, 1, r, stdout) != r) {
            fclose(out);
            remove(TEMP_INPUT); remove(TEMP_OUTPUT);
            fprintf(stderr, "stdout write failed\n");
            return 1;
        }
    }
    fclose(out);
    fflush(stdout);

    remove(TEMP_INPUT);
    remove(TEMP_OUTPUT);
    fprintf(stderr, "Conversion to %s complete\n", format);
    return 0;
}
