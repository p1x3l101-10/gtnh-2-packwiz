inputs: inputs.flake-utils.lib.eachDefaultSystem (system:
  let
    pkgs = inputs.nixpkgs.legacyPackages.${system};
  in {
    formatter = pkgs.nixpkgs-fmt;
    packages = inputs.system-lib.lib.flake.genPackages ./packages pkgs.newScope {
      internal = {
        inherit (inputs) systems cmake-libs;
        metadata = builtins.fromTOML (builtins.readFile ../metadata.toml);
      };
    };
    apps = {
      format = {
        type = "app";
        program = pkgs.writeShellApplication {
          name = "formatter";
          runtimeInputs = with pkgs; [
            llvmPackages_20.clang-tools
          ];
          text = ''
            clang-format
          '';
        };
      };
    };
  }
)