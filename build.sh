#! /usr/bin/env bash

build(){
	cmake -E remove CMakeCache.txt
	cmake -E remove_directory CMakeFiles
	
	cmake -E make_directory build
	cmake -E chdir build cmake ..
}

build || echo "Build failed" 
