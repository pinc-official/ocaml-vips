type image

type format =
  [ `jpg
  | `png
  | `webp
  | `avif
  | `tiff
  | `gif
  ]

(** Resampling kernel used during resize. [Lanczos3] is the default and gives
    the best quality for downscaling. [Nearest] is fastest. *)
type kernel =
  [ `Nearest
  | `Linear
  | `Cubic
  | `Mitchell
  | `Lanczos2
  | `Lanczos3
  ]

(** Which part of the image is "interesting" for smart-crop. *)
type interesting =
  [ `None
  | `Center
  | `Entropy
  | `Attention
  ]
