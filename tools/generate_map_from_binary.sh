#!/bin/bash

readelf_path=readelf
min_address=1

function print_help {
    echo "Usage: ./generate_map_from_binary.sh [OPTION]..."
    echo "Generate hawktracer map output with function addresses from binary file"
    echo ""
    echo "  --binary=BINARY_FILE        path to a binary file"
    echo "  --readelf=READELF_BIN       path to a readelf application"
    echo "  --min-address=VALUE         minimum functinon address that will be"
    echo "                              considered in the output"
}

for i in "$@"
do
    case $i in
        --readelf=*)
            readelf_path="${i#*=}"
            ;;
        --binary=*)
            binary_path="${i#*=}"
            ;;
	--min-address=*)
	    min_address=$((${i#*=}))
	    ;;
        *)
            print_help
            exit
            ;;
    esac
    shift
done

$readelf_path -s --wide $binary_path | grep " FUNC " | gawk '{print strtonum("0x" $2) " " $8}' | awk '{system("c++filt" $3)}' | grep '^[0-9]' | awk -v min_address=$min_address '{ if ($1 >= min_address) print $1 " 0 " $2 }' 
