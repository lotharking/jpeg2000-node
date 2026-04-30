#include <openjpeg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#define EXPORT EMSCRIPTEN_KEEPALIVE
#else
#define EXPORT
#endif

// --- OpenJPEG callbacks ---
static void error_callback(const char *msg, void *client_data) { (void)client_data; fprintf(stderr, "[ERROR] %s\n", msg); }
static void warning_callback(const char *msg, void *client_data) { (void)client_data; fprintf(stderr, "[WARNING] %s\n", msg); }

// --- In-memory stream ---
typedef struct {
    const uint8_t *data;
    size_t length;
    size_t position;
} mem_stream_t;

static OPJ_SIZE_T mem_stream_read(void *buf, OPJ_SIZE_T nb, void *udata) {
    mem_stream_t *ms = (mem_stream_t *)udata;
    size_t remaining = ms->length - ms->position;
    if (remaining == 0) return (OPJ_SIZE_T)-1;
    OPJ_SIZE_T to_read = (nb < remaining) ? nb : (OPJ_SIZE_T)remaining;
    memcpy(buf, ms->data + ms->position, to_read);
    ms->position += to_read;
    return to_read;
}

static OPJ_OFF_T mem_stream_skip(OPJ_OFF_T nb, void *udata) {
    mem_stream_t *ms = (mem_stream_t *)udata;
    size_t remaining = ms->length - ms->position;
    if ((size_t)nb > remaining) nb = (OPJ_OFF_T)remaining;
    ms->position += (size_t)nb;
    return nb;
}

static OPJ_BOOL mem_stream_seek(OPJ_OFF_T nb, void *udata) {
    mem_stream_t *ms = (mem_stream_t *)udata;
    if ((size_t)nb > ms->length) return OPJ_FALSE;
    ms->position = (size_t)nb;
    return OPJ_TRUE;
}

// --- Public API ---

/*
 * Decodes a JP2 image from memory.
 * Returns a malloc'd RGB(A) pixel buffer. Caller must free via free_buffer().
 * On error returns NULL. out_width/out_height/out_channels are set on success.
 */
EXPORT uint8_t *decode_jp2(const uint8_t *input, int input_len,
                            int *out_width, int *out_height, int *out_channels) {
    if (!input || input_len <= 0) return NULL;

    mem_stream_t ms = { input, (size_t)input_len, 0 };

    opj_codec_t *codec = opj_create_decompress(OPJ_CODEC_JP2);
    if (!codec) return NULL;

    opj_set_error_handler(codec, error_callback, NULL);
    opj_set_warning_handler(codec, warning_callback, NULL);

    opj_dparameters_t params;
    opj_set_default_decoder_parameters(&params);
    if (!opj_setup_decoder(codec, &params)) {
        opj_destroy_codec(codec);
        return NULL;
    }

    opj_stream_t *stream = opj_stream_create((OPJ_SIZE_T)input_len, OPJ_TRUE);
    if (!stream) {
        opj_destroy_codec(codec);
        return NULL;
    }

    opj_stream_set_user_data(stream, &ms, NULL);
    opj_stream_set_user_data_length(stream, (OPJ_UINT64)input_len);
    opj_stream_set_read_function(stream, mem_stream_read);
    opj_stream_set_skip_function(stream, mem_stream_skip);
    opj_stream_set_seek_function(stream, mem_stream_seek);

    opj_image_t *image = NULL;
    int ok = opj_read_header(stream, codec, &image)
          && opj_decode(codec, stream, image)
          && opj_end_decompress(codec, stream);

    opj_stream_destroy(stream);
    opj_destroy_codec(codec);

    if (!ok) {
        if (image) opj_image_destroy(image);
        return NULL;
    }

    if (image->numcomps < 3) {
        opj_image_destroy(image);
        return NULL;
    }

    int width    = (int)image->comps[0].w;
    int height   = (int)image->comps[0].h;
    int channels = (image->numcomps >= 4) ? 4 : 3;

    for (int i = 1; i < channels; i++) {
        if ((int)image->comps[i].w != width || (int)image->comps[i].h != height) {
            opj_image_destroy(image);
            return NULL;
        }
    }

    uint8_t *pixels = (uint8_t *)malloc((size_t)(width * height * channels));
    if (!pixels) {
        opj_image_destroy(image);
        return NULL;
    }

    int max_val = (1 << image->comps[0].prec) - 1;
    for (int i = 0; i < width * height; i++) {
        for (int c = 0; c < channels; c++) {
            int val = image->comps[c].data[i];
            if (image->comps[c].sgnd) val += (max_val + 1) / 2;
            val = (val * 255) / max_val;
            if (val < 0) val = 0;
            if (val > 255) val = 255;
            pixels[i * channels + c] = (uint8_t)val;
        }
    }

    opj_image_destroy(image);

    *out_width    = width;
    *out_height   = height;
    *out_channels = channels;
    return pixels;
}

EXPORT void free_buffer(uint8_t *ptr) {
    free(ptr);
}
