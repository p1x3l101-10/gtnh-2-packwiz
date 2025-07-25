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
    apps = let
      llvm = pkgs.llvmPackages_20;
      inherit (inputs.flake-utils.lib) mkApp;
    in {
      format = mkApp { drv = (pkgs.writeShellApplication {
        name = "formatter";
        runtimeInputs = with pkgs; [
          llvm.clang-tools
          findutils
        ];
        text = ''
          find . \( -name '*.hpp' -or -name '*.cpp' \) -print0 | xargs -0 clang-format -i
        '';
      });};
      clangd = mkApp { drv = (pkgs.writeShellApplication {
        name = "clangd";
        runtimeInputs = with pkgs; [
          llvm.clang-tools
        ];
        text = ''
          exec clangd "$@"
        '';
      });};
    };
  }
)