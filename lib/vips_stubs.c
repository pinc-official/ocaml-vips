#include <caml/mlvalues.h>
#include <caml/memory.h>
#include <caml/alloc.h>
#include <caml/fail.h>
#include <caml/custom.h>

#include <vips/vips.h>
#include <string.h>
#include <stdlib.h>

/* ------------------------------------------------------------------ */
/* Error helpers                                                      */
/* ------------------------------------------------------------------ */

static void raise_vips_error(const char *context) {
    char buf[1024];
    snprintf(buf, sizeof(buf), "%s: %s", context, vips_error_buffer());
    vips_error_clear();
    caml_failwith(buf);
}

/* ------------------------------------------------------------------ */
/* VipsImage custom block                                             */
/* ------------------------------------------------------------------ */

static void vips_image_finalize(value v) {
    VipsImage *img = *((VipsImage **)Data_custom_val(v));
    if (img) {
        g_object_unref(img);
    }
}

static struct custom_operations vips_image_ops = {
    "vips_image",
    vips_image_finalize,
    custom_compare_default,
    custom_hash_default,
    custom_serialize_default,
    custom_deserialize_default,
    custom_compare_ext_default,
    custom_fixed_length_default
};

static value alloc_vips_image(VipsImage *img) {
    value v = caml_alloc_custom(&vips_image_ops, sizeof(VipsImage *), 0, 1);
    *((VipsImage **)Data_custom_val(v)) = img;
    return v;
}

static VipsImage *get_vips_image(value v) {
    return *((VipsImage **)Data_custom_val(v));
}

/* ------------------------------------------------------------------ */
/* Init / Shutdown                                                    */
/* ------------------------------------------------------------------ */

CAMLprim value caml_vips_init(value prog_name) {
    CAMLparam1(prog_name);
    if (VIPS_INIT(String_val(prog_name))) {
        raise_vips_error("vips_init");
    }
    CAMLreturn(Val_unit);
}

CAMLprim value caml_vips_shutdown(value unit) {
    CAMLparam1(unit);
    vips_shutdown();
    CAMLreturn(Val_unit);
}

/* ------------------------------------------------------------------ */
/* Input                                                              */
/* ------------------------------------------------------------------ */

CAMLprim value caml_vips_image_new_from_file(value path) {
    CAMLparam1(path);
    CAMLlocal1(result);

    VipsImage *img = vips_image_new_from_file(String_val(path), NULL);
    if (!img) {
        raise_vips_error("vips_image_new_from_file");
    }
    result = alloc_vips_image(img);
    CAMLreturn(result);
}

CAMLprim value caml_vips_image_new_from_buffer(value buf) {
    CAMLparam1(buf);
    CAMLlocal1(result);

    VipsImage *img = vips_image_new_from_buffer(
        Bytes_val(buf), caml_string_length(buf), "", NULL);
    if (!img) {
        raise_vips_error("vips_image_new_from_buffer");
    }
    result = alloc_vips_image(img);
    CAMLreturn(result);
}

/* ------------------------------------------------------------------ */
/* Output                                                             */
/* ------------------------------------------------------------------ */

CAMLprim value caml_vips_image_write_to_file(value img_val, value path_val, value format_val, value quality_val) {
    CAMLparam4(img_val, path_val, format_val, quality_val);

    VipsImage *img    = get_vips_image(img_val);
    const char *path  = String_val(path_val);
    int format        = Int_val(format_val);
    int quality       = Int_val(quality_val); /* 1-100; -1 = use default */
    int ret = -1;

    switch (format) {
        case 0: /* JPEG */
            ret = (quality >= 0)
                ? vips_jpegsave(img, path, "Q", quality, NULL)
                : vips_jpegsave(img, path, NULL);
            break;
        case 1: /* PNG */
            ret = vips_pngsave(img, path, NULL);
            break;
        case 2: /* WebP */
            ret = (quality >= 0)
                ? vips_webpsave(img, path, "Q", quality, NULL)
                : vips_webpsave(img, path, NULL);
            break;
        case 3: /* AVIF */
            ret = (quality >= 0)
                ? vips_heifsave(img, path, "Q", quality, "compression",
                                VIPS_FOREIGN_HEIF_COMPRESSION_AV1, NULL)
                : vips_heifsave(img, path, "compression",
                                VIPS_FOREIGN_HEIF_COMPRESSION_AV1, NULL);
            break;
        case 4: /* TIFF */
            ret = vips_tiffsave(img, path, NULL);
            break;
        case 5: /* GIF */
            ret = vips_gifsave(img, path, NULL);
            break;
        default:
            caml_failwith("caml_vips_image_write_to_file: unknown format");
    }

    if (ret != 0) raise_vips_error("vips_image_write_to_file");
    CAMLreturn(Val_unit);
}

CAMLprim value caml_vips_image_write_to_buffer(value img_val, value format_val, value quality_val) {
    CAMLparam3(img_val, format_val, quality_val);
    CAMLlocal1(result);

    VipsImage *img = get_vips_image(img_val);
    int format     = Int_val(format_val);
    int quality    = Int_val(quality_val);
    void *buf      = NULL;
    size_t len     = 0;
    int ret        = -1;

    switch (format) {
        case 0: /* JPEG */
            ret = (quality >= 0)
                ? vips_jpegsave_buffer(img, &buf, &len, "Q", quality, NULL)
                : vips_jpegsave_buffer(img, &buf, &len, NULL);
            break;
        case 1: /* PNG */
            ret = vips_pngsave_buffer(img, &buf, &len, NULL);
            break;
        case 2: /* WebP */
            ret = (quality >= 0)
                ? vips_webpsave_buffer(img, &buf, &len, "Q", quality, NULL)
                : vips_webpsave_buffer(img, &buf, &len, NULL);
            break;
        case 3: /* AVIF */
            ret = (quality >= 0)
                ? vips_heifsave_buffer(img, &buf, &len, "Q", quality,
                                       "compression",
                                       VIPS_FOREIGN_HEIF_COMPRESSION_AV1, NULL)
                : vips_heifsave_buffer(img, &buf, &len, "compression",
                                       VIPS_FOREIGN_HEIF_COMPRESSION_AV1, NULL);
            break;
        case 4: /* TIFF */
            ret = vips_tiffsave_buffer(img, &buf, &len, NULL);
            break;
        default:
            caml_failwith("caml_vips_image_write_to_buffer: unsupported format");
    }

    if (ret != 0) raise_vips_error("vips_image_write_to_buffer");

    result = caml_alloc_string(len);
    memcpy(Bytes_val(result), buf, len);
    g_free(buf);

    CAMLreturn(result);
}

/* ------------------------------------------------------------------ */
/* Metadata                                                           */
/* ------------------------------------------------------------------ */

CAMLprim value caml_vips_image_get_width(value img_val) {
    CAMLparam1(img_val);
    CAMLreturn(Val_int(vips_image_get_width(get_vips_image(img_val))));
}

CAMLprim value caml_vips_image_get_height(value img_val) {
    CAMLparam1(img_val);
    CAMLreturn(Val_int(vips_image_get_height(get_vips_image(img_val))));
}

CAMLprim value caml_vips_image_get_bands(value img_val) {
    CAMLparam1(img_val);
    CAMLreturn(Val_int(vips_image_get_bands(get_vips_image(img_val))));
}

/* ------------------------------------------------------------------ */
/* Transform                                                          */
/* ------------------------------------------------------------------ */

CAMLprim value caml_vips_resize(value img_val, value width_val, value height_val, value kernel_val) {
    CAMLparam4(img_val, width_val, height_val, kernel_val);
    CAMLlocal1(result);

    VipsImage *img    = get_vips_image(img_val);
    int target_w      = Int_val(width_val);
    int target_h      = Int_val(height_val); /* -1 = maintain aspect ratio */
    int kernel_int    = Int_val(kernel_val);
    VipsKernel kernel;

    switch (kernel_int) {
        case 0:  kernel = VIPS_KERNEL_NEAREST;  break;
        case 1:  kernel = VIPS_KERNEL_LINEAR;   break;
        case 2:  kernel = VIPS_KERNEL_CUBIC;    break;
        case 3:  kernel = VIPS_KERNEL_MITCHELL; break;
        case 4:  kernel = VIPS_KERNEL_LANCZOS2; break;
        default: kernel = VIPS_KERNEL_LANCZOS3; break;
    }

    int src_w = vips_image_get_width(img);
    double hscale = (double)target_w / (double)src_w;

    VipsImage *out = NULL;

    if (target_h < 0) {
        /* Uniform scale - maintain aspect ratio */
        if (vips_resize(img, &out, hscale, "kernel", kernel, NULL) != 0)
            raise_vips_error("vips_resize");
    } else {
        /* Scale to exact dimensions - may change aspect ratio */
        int src_h  = vips_image_get_height(img);
        double vscale = (double)target_h / (double)src_h;
        if (vips_resize(img, &out, hscale, "vscale", vscale, "kernel", kernel, NULL) != 0)
            raise_vips_error("vips_resize (with vscale)");
    }

    result = alloc_vips_image(out);
    CAMLreturn(result);
}

CAMLprim value caml_vips_thumbnail_image(value img_val, value width_val, value height_val) {
    CAMLparam3(img_val, width_val, height_val);
    CAMLlocal1(result);

    VipsImage *img    = get_vips_image(img_val);
    int target_w      = Int_val(width_val);
    int target_h      = Int_val(height_val);
    VipsImage *out    = NULL;
    int ret;

    if (target_w < 0 && target_h < 0) {
        caml_failwith("vips_thumbnail_image: at least one of width or height must be set");
    } else if (target_w < 0) {
        ret = vips_thumbnail_image(img, &out, VIPS_MAX_COORD,
                                   "height", target_h, NULL);
    } else if (target_h < 0) {
        ret = vips_thumbnail_image(img, &out, target_w, NULL);
    } else {
        ret = vips_thumbnail_image(img, &out, target_w,
                                   "height", target_h,
                                   "crop", VIPS_INTERESTING_ATTENTION,
                                   NULL);
    }

    if (ret != 0) raise_vips_error("vips_thumbnail_image");
    result = alloc_vips_image(out);
    CAMLreturn(result);
}

CAMLprim value caml_vips_crop(value img_val, value left_val, value top_val, value w_val, value h_val) {
    CAMLparam5(img_val, left_val, top_val, w_val, h_val);
    CAMLlocal1(result);

    VipsImage *img = get_vips_image(img_val);
    VipsImage *out = NULL;

    if (vips_extract_area(img, &out,
                          Int_val(left_val), Int_val(top_val),
                          Int_val(w_val),    Int_val(h_val),
                          NULL) != 0)
        raise_vips_error("vips_extract_area");

    result = alloc_vips_image(out);
    CAMLreturn(result);
}

CAMLprim value caml_vips_smartcrop(value img_val, value w_val, value h_val, value interesting_val) {
    CAMLparam4(img_val, w_val, h_val, interesting_val);
    CAMLlocal1(result);

    VipsImage *img = get_vips_image(img_val);
    VipsImage *out = NULL;
    VipsInteresting interesting;

    switch (Int_val(interesting_val)) {
        case 0:  interesting = VIPS_INTERESTING_NONE;      break;
        case 1:  interesting = VIPS_INTERESTING_CENTRE;    break;
        case 2:  interesting = VIPS_INTERESTING_ENTROPY;   break;
        default: interesting = VIPS_INTERESTING_ATTENTION; break;
    }

    if (vips_smartcrop(img, &out, Int_val(w_val), Int_val(h_val),
                       "interesting", interesting, NULL) != 0)
        raise_vips_error("vips_smartcrop");

    result = alloc_vips_image(out);
    CAMLreturn(result);
}

CAMLprim value caml_vips_rotate(value img_val, value angle_val) {
    CAMLparam2(img_val, angle_val);
    CAMLlocal1(result);

    VipsImage *img = get_vips_image(img_val);
    VipsImage *out = NULL;
    VipsAngle angle;

    switch (Int_val(angle_val)) {
        case 90:  angle = VIPS_ANGLE_D90;  break;
        case 180: angle = VIPS_ANGLE_D180; break;
        case 270: angle = VIPS_ANGLE_D270; break;
        default:  CAMLreturn(img_val); /* no-op */
    }

    if (vips_rot(img, &out, angle, NULL) != 0)
        raise_vips_error("vips_rot");

    result = alloc_vips_image(out);
    CAMLreturn(result);
}

CAMLprim value caml_vips_flip_horizontal(value img_val) {
    CAMLparam1(img_val);
    CAMLlocal1(result);
    VipsImage *out = NULL;
    if (vips_flip(get_vips_image(img_val), &out, VIPS_DIRECTION_HORIZONTAL, NULL) != 0)
        raise_vips_error("vips_flip horizontal");
    result = alloc_vips_image(out);
    CAMLreturn(result);
}

CAMLprim value caml_vips_flip_vertical(value img_val) {
    CAMLparam1(img_val);
    CAMLlocal1(result);
    VipsImage *out = NULL;
    if (vips_flip(get_vips_image(img_val), &out, VIPS_DIRECTION_VERTICAL, NULL) != 0)
        raise_vips_error("vips_flip vertical");
    result = alloc_vips_image(out);
    CAMLreturn(result);
}

CAMLprim value caml_vips_autorotate(value img_val) {
    CAMLparam1(img_val);
    CAMLlocal1(result);
    VipsImage *out = NULL;
    if (vips_autorot(get_vips_image(img_val), &out, NULL) != 0)
        raise_vips_error("vips_autorot");
    result = alloc_vips_image(out);
    CAMLreturn(result);
}
