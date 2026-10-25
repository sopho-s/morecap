for i in "$@"; do
  case $i in
    -k=*|--kernel-version=*)
      KERNELVERSION="${i#*=}"
      shift # past argument=value
      ;;
    -s=*|--share-dir=*)
      SHAREDIR="${i#*=}"
      shift # past argument=value
      ;;
    -*|--*)
      echo "Unknown option $i"
      exit 1
      ;;
    *)
      ;;
  esac
done

LINUX="linux-${KERNELVERSION}"

if [ ! -d ./${LINUX} ]; then
    echo "downloading linux..."
    wget https://cdn.kernel.org/pub/linux/kernel/v6.x/${LINUX}.tar.xz
    tar xf "./${LINUX}.tar.xz"
    rm -f "./${LINUX}.tar.xz"
fi

cd ./$LINUX

if [ ! -f ./arch/x86/boot/bzImage ]; then
    virtme-configkernel --defconfig
    if ! grep -q '^CONFIG_VIRTIO_CONSOLE=y' .config; then
        scripts/config -e VIRTIO_CONSOLE
        make olddefconfig
    fi

    make -j $(nproc)
fi

cd ..

export -p > .nixenv.sh

cat > "$(pwd)/inenv.sh" <<EOF

cd $(pwd)/

source $(pwd)/.nixenv.sh
rm -rf $(pwd)/build
cmake -B $(pwd)/build -S ./
cmake --build $(pwd)/build
(insmod $(pwd)/build/morecap.ko && echo "loaded module" && exec bash -i) || { dmesg; }
EOF

virtme-run --kdir ./$LINUX --rwdir ../$SHAREDIR --rwdir /tmp --mods none --memory 4G --shell "$(pwd)/inenv.sh"