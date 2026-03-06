type image

type format =
  [ `avif
  | `gif
  | `jpg
  | `png
  | `tiff
  | `webp
  ]

type kernel =
  [ `Cubic
  | `Lanczos2
  | `Lanczos3
  | `Linear
  | `Mitchell
  | `Nearest
  ]

type interesting =
  [ `Attention
  | `Center
  | `Entropy
  | `None
  ]

val init : string -> unit
val shutdown : unit -> unit

module Input : sig
  val load_file : string -> image
  val load_buffer : bytes -> image
  val load_fd : Unix.file_descr -> image
end

module Output : sig
  val save_file : ?quality:int -> path:string -> format:format -> image -> unit

  val save_buffer : ?quality:int -> format:format -> image -> bytes
end

module Metadata : sig
  val width : image -> int
  val height : image -> int
  val dimensions : image -> int * int
end

module Transform : sig
  val resize_width : image -> width:int -> ?kernel:kernel -> unit -> image
  val resize_height : image -> height:int -> ?kernel:kernel -> unit -> image

  (** Produce a thumbnail from [img].
    - [~width] only → scale to width, preserve aspect ratio
    - [~height] only → scale to height, preserve aspect ratio
    - both → scale and smart-crop (attention-based) to exact dimensions *)
  val thumbnail : ?width:int -> ?height:int -> image -> image

  val crop : image -> left:int -> top:int -> width:int -> height:int -> image

  val smartcrop
    :  image
    -> width:int
    -> height:int
    -> ?interesting:[ `Attention | `Center | `Entropy | `None ]
    -> unit
    -> image

  val rotate : image -> int -> image

  val autorotate : image -> image

  val flip_horizontal : image -> image

  val flip_vertical : image -> image
end

module Format : sig
  val of_path : string -> format option
end
