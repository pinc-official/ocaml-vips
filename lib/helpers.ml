let format_to_int = function
  | `jpg -> 0
  | `png -> 1
  | `webp -> 2
  | `avif -> 3
  | `tiff -> 4
  | `gif -> 5
;;

let kernel_to_int = function
  | `Nearest -> 0
  | `Linear -> 1
  | `Cubic -> 2
  | `Mitchell -> 3
  | `Lanczos2 -> 4
  | `Lanczos3 -> 5
;;

let interesting_to_int = function
  | `None -> 0
  | `Center -> 1
  | `Entropy -> 2
  | `Attention -> 3
;;
