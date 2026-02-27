(lang dune 3.0)

(name ocaml-vips)

(generate_opam_files true)

(source
 (github pinc-official/ocaml-vips))

(license MIT)

(authors "Torben Ewert")

(maintainers "torben@ewert-online.com")

(package
 (name ocaml-vips)
 (synopsis "OCaml bindings for libvips image processing")
 (depends
  (ocaml
   (>= 4.14))
  dune
  dune-configurator
  ctypes
  ctypes-foreign
  conf-vips
  (ocamlformat :with-dev-setup)
  (ocaml-lsp-server :with-dev-setup)))
