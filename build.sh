#! /usr/bin/env bash

build(){
	cmake -E remove CMakeCache.txt
	cmake -E remove_directory CMakeFiles
	
	cmake -E make_directory build
	cmake -E chdir build cmake ..


	if [[ $run == true ]]; then
		exec ./build/bin/vsrg-client
	fi
}

run=false;
if [[ "$1" == "run" ]]; then
	run=true;
fi

build || echo "Build failed" && exit 1;


