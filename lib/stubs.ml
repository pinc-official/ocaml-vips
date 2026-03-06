open Types

external init : string -> unit = "caml_vips_init"
external shutdown : unit -> unit = "caml_vips_shutdown"

external load_file : string -> image = "caml_vips_image_new_from_file"
external load_buffer : bytes -> image = "caml_vips_image_new_from_buffer"
external load_fd : Unix.file_descr -> image = "caml_vips_image_new_from_fd"

external save_file
  :  image
  -> string
  -> int
  -> int
  -> unit
  = "caml_vips_image_write_to_file"
external save_buffer : image -> int -> int -> bytes = "caml_vips_image_write_to_buffer"

external width : image -> int = "caml_vips_image_get_width"
external height : image -> int = "caml_vips_image_get_height"
external bands : image -> int = "caml_vips_image_get_bands"

external resize_width : image -> int -> int -> image = "caml_vips_resize_width"
external resize_height : image -> int -> int -> image = "caml_vips_resize_height"
external thumbnail_image : image -> int -> int -> image = "caml_vips_thumbnail_image"
external crop : image -> int -> int -> int -> int -> image = "caml_vips_crop"
external smartcrop : image -> int -> int -> int -> image = "caml_vips_smartcrop"

external rotate : image -> int -> image = "caml_vips_rotate"
external flip_h : image -> image = "caml_vips_flip_horizontal"
external flip_v : image -> image = "caml_vips_flip_vertical"
external autorotate : image -> image = "caml_vips_autorotate"
