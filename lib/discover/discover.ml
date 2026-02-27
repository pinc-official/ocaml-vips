module C = Configurator.V1

let () =
  C.main ~name:"vips" (fun c ->
    let default = { C.Pkg_config.cflags = []; libs = [] } in
    let conf =
      match C.Pkg_config.get c with
      | None -> default
      | Some pc ->
        (match C.Pkg_config.query pc ~package:"vips" with
         | None -> default
         | Some deps -> deps)
    in
    C.Flags.write_sexp "vips_c_compile_flags.sexp" conf.cflags;
    C.Flags.write_sexp "vips_c_flags.sexp" conf.libs)
;;
