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
end

module Output : sig
  val save_file : image -> string -> format -> ?quality:int -> unit -> unit

  val save_buffer : image -> format -> ?quality:int -> unit -> bytes
end

module Metadata : sig
  val width : image -> int
  val height : image -> int
  val dimensions : image -> int * int
end

module Transform : sig
  val resize : image -> width:int -> ?height:int -> ?kernel:kernel -> unit -> image

  (** Produce a thumbnail from [img].
    - [~width] only → scale to width, preserve aspect ratio
    - [~height] only → scale to height, preserve aspect ratio
    - both → scale and smart-crop (attention-based) to exact dimensions At least
      one of [~width] or [~height] must be provided. *)
  val thumbnail : image -> ?width:int -> ?height:int -> unit -> image

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
