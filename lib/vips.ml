include Types

let init program = Stubs.init program
let shutdown () = Stubs.shutdown ()

module Input = struct
  let load_file path = Stubs.load_file path

  let load_buffer buf = Stubs.load_buffer buf
end

module Output = struct
  let save_file img path format ?(quality = -1) () =
    Stubs.save_file img path (Helpers.format_to_int format) quality
  ;;

  let save_buffer img format ?(quality = -1) () =
    Stubs.save_buffer img (Helpers.format_to_int format) quality
  ;;
end

module Metadata = struct
  let width img = Stubs.width img
  let height img = Stubs.height img
  let dimensions img = Stubs.width img, Stubs.height img
end

module Transform = struct
  let resize img ~width ?(height = -1) ?(kernel = `Lanczos3) () =
    Stubs.resize img width height (Helpers.kernel_to_int kernel)
  ;;

  let thumbnail img ?width ?height () =
    let w = Option.value width ~default:(-1) in
    let h = Option.value height ~default:(-1) in
    Stubs.thumbnail_image img w h
  ;;

  let crop img ~left ~top ~width ~height = Stubs.crop img left top width height

  let smartcrop img ~width ~height ?(interesting = `Attention) () =
    Stubs.smartcrop img width height (Helpers.interesting_to_int interesting)
  ;;

  let rotate img angle = Stubs.rotate img angle

  let autorotate img = Stubs.autorotate img

  let flip_horizontal img = Stubs.flip_h img
  let flip_vertical img = Stubs.flip_v img
end

module Format = struct
  let of_path path =
    match Filename.extension (String.lowercase_ascii path) with
    | ".jpg" | ".jpeg" -> Some `jpg
    | ".png" -> Some `png
    | ".webp" -> Some `webp
    | ".avif" -> Some `avif
    | ".tiff" | ".tif" -> Some `tiff
    | ".gif" -> Some `gif
    | _ -> None
  ;;
end
