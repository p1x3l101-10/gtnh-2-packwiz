{ lib
, internal
, llvmPackages_20
, stdenvNoCC
, toml-cli
, pkgconf
, cmakeMinimal
# Project Deps
, boost
, magic-enum
, nlohmann_json
, tomlplusplus
, log4cpp
, curlpp
, cryptopp
, curl
}:

let
  stdenv = llvmPackages_20.stdenv;
  src = stdenvNoCC.mkDerivation {
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
              (fs.maybeMissing (baseDir + "/build"))
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
    pkgconf
  ];

  buildInputs = [
    boost
    magic-enum
    nlohmann_json
    tomlplusplus
    log4cpp
    curlpp
    cryptopp
  ] ++ [ # Deps not in search path???
    curl
  ];

  cmakeFlags = [
    "-DBAD_CURLPP_VERSION_FORMAT=TRUE"
    "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON"
  ];

  meta = with lib; {
    inherit (internal.metadata) homepage description;
    license = licenses.bsd3;
    platforms = import internal.systems;
  };
}