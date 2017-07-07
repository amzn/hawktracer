#!/bin/bash

readelf_path=readelf
min_address=1

echoerr() { echo "$@" 1>&2; }

function print_help {
    echoerr "Usage: ./generate_map_from_binary.sh [OPTION]..."
    echoerr "Generate hawktracer map output with function addresses from binary file"
    echoerr ""
    echoerr "  --binary=BINARY_FILE        path to a binary file"
    echoerr "  --readelf=READELF_BIN       path to a readelf application"
    echoerr "  --min-address=VALUE         minimum functinon address that will be"
    echoerr "                              considered in the output"
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

if [ -z "$binary_path" ]
then
    echoerr "Option --binary is required!"
    exit 1
fi

echoerr "Generating address map for $binary_path"
echoerr "Minimum address: $min_address"
echoerr "Readelf application: $readelf_path"

$readelf_path -s --wide $binary_path | grep " FUNC " | gawk '{print strtonum("0x" $2) " " $8}' | awk '{system("c++filt" $3)}' | grep '^[0-9]' | awk -v min_address=$min_address '{ if ($1 >= min_address) print $1 " 2 " $2 }' 

echoerr "Generating map completed!"
