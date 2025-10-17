{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    systems.url = "github:nix-systems/x86_64-linux";
    flake-utils = {
      url = "github:numtide/flake-utils";
      inputs.systems.follows = "systems";
    };
    # Use my custom lib, large flake, but still a useful lib
    system-lib = {
      url = "github:p1x3l101-10/nixos?dir=lib";
      inputs = {
        nixpkgs.follows = "nixpkgs";
        systems.follows = "systems";
        flake-utils.follows = "flake-utils";
      };
    };
    # Program deps
    cmake-libs = {
      url = "github:p1x3l101-10/cmake-libs";
      flake = false;
    };
  };
  outputs = inputs: import ./nix/outputs.nix inputs;
}
