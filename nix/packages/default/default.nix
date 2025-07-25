{ lib
, internal
, stdenv
, toml-cli
, pkg-config
, cmakeMinimal
# Project Deps
, boost
, magic-enum
, nlohmann_json
, tomlplusplus
, log4cpp
, curlpp
, cryptopp
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
      if [[ -e root/cmake-libs ]]; then
        rm -rvf root/cmake-libs
      fi
      mv -v source root/cmake-libs 
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
    pkg-config
  ];

  buildInputs = [
    boost
    magic-enum
    nlohmann_json
    tomlplusplus
    log4cpp
    curlpp
    cryptopp
  ];

  meta = with lib; {
    inherit (internal.metadata) homepage description;
    license = licenses.bsd3;
    platforms = import internal.systems;
  };
}