{ pkgs ? import <nixpkgs> {} }:
let
  kernel = pkgs.linuxPackages.kernel;   # match your running kernel, e.g. linuxPackages_latest
in pkgs.mkShell {
  nativeBuildInputs = with pkgs; [
    gnumake 
    gcc
    qemu
    wget
    flex
    bison
    bc
    perl
    debootstrap
  ];

  

  buildInputs = with pkgs; [
    openssl
    elfutils
    ncurses
  ];

  packages = with pkgs; [
    (python3.withPackages (ps: with ps; [ pip argcomplete requests setuptools ]))
    virtiofsd
    busybox
  ];

  hardeningDisable = [ "all" ];

  KERNELDIR = toString ./linux-6.18.55;
  shellHook = ''
    if [ ! -d .venv ]; then
      python3 -m venv --system-site-packages .venv
      .venv/bin/pip install --quiet virtme-ng
    fi
    source .venv/bin/activate
  '';
}