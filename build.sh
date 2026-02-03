#! /usr/bin/env bash

build(){
	if [[ $clean == true ]]; then
		rm -rf out/
	fi

	if [[ $release == true ]]; then
		PRESET="linux-release"
	else
		PRESET="linux-debug"
	fi

	cmake --preset "$PRESET" || { echo "CMake configuration failed"; exit 1; }
	cmake --build --preset "$PRESET" || { echo "Build process failed"; exit 1; }

	if [[ $run == true ]]; then
		exec ./out/build/$PRESET/bin/vsrg-client
	fi
}

run=false;
clean=false;
release=false;

for arg in "$@"; do
	case $arg in
		--run)
			run=true
			shift
			;;
		--clean)
			clean=true
			shift
			;;
		--release)
			release=true
			shift
			;;
	esac
done

build || echo "Build failed" && exit 1;