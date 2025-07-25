{ lib
, internal
, stdenv
, toml-cli
, cmakeMinimal
}:

let
  src = stdenv.mkDerivation {
    name = "sources-merged";
    srcs = let
      fs = lib.fileset;
      baseDir = ../../..;
    in [
      internal.cmake-libs
      (lib.sources.cleanSourceWith {
        name = "root";
        src = (fs.toSource {
          root = baseDir;
          fileset = (fs.difference
            (fs.gitTracked baseDir)
            (fs.unions [
              (fs.maybeMissing (baseDir + "/result"))
              (baseDir + "/flake.lock")
              (baseDir + "/flake.nix")
              (baseDir + "/nix")
            ])
          );
        });
      })
    ];
    sourceRoot = ".";
    buildPhase = ''
      mv source root/cmake-libs 
    '';
    installPhase = ''
      mv root $out
    '';
  };
in

stdenv.mkDerivation {
  inherit (internal.metadata) pname version;
  inherit src;

  nativeBuildInputs = [
    cmakeMinimal
    toml-cli
  ];

  buildInputs = [];

  meta = with lib; {
    inherit (internal.metadata) homepage description;
    license = licenses.bsd3;
    platforms = import internal.systems;
  };
}