#!/bin/bash

function get_parsed_data {
    prev_ifs=$IFS
    IFS=',' read -r -a array <<< "$1"
    IFS=$prev_IFS
    echo "${array[$2]}"
}

function get_category {
    prev_ifs=$IFS
    IFS=' ' read -r -a array <<< "$1"
    IFS=$prev_IFS
}

function generate_cpp_header {
    local out_file="$1"
    local labels_array="$2[@]"

    >$out_file

    echo "#ifndef HAWKTRACER_ACTIONLABELS_H_" >> $out_file
    echo "#define HAWKTRACER_ACTIONLABELS_H_" >> $out_file
    echo "" >> $out_file

    value=1
    for label in "${!labels_array}"
    do
	label=$(get_parsed_data $label 0)
	echo "#define hawktracer_tracepoint_$label $value" >> $out_file
	((value+=1))
    done
    
    echo "" >> $out_file
    echo "#endif // HAWKTRACER_ACTIONLABELS_H_" >> $out_file
    echo "" >> $out_file
}

function generate_map {
    local out_file="$1"
    local labels_array="$2[@]"
    
    >$out_file

    value=1
    for label in "${!labels_array}"
    do
	category=$(get_parsed_data $label 1)
	label=$(get_parsed_data $label 0)
	echo "$value $category $label" >> $out_file
	((value+=1))
    done
}

function generate_lua_variables {
    local labels_array="$1[@]"
    local array_size=$2

    if [[ $array_size -ne 0 ]]
    then
	values_left=""
	values_right=""
	value=1
	for label in "${!labels_array}"
	do
	    label=$(get_parsed_data $label 0)
	    values_left=$values_left$label","
	    values_right=$values_right$value","
	    ((value+=1))
	done
	values_left=${values_left::-1}
	values_right=${values_right::-1}
	echo "local " $values_left "=" $values_right
    fi
}

labels=()
cpp_regex="TRACEPOINT_NAMED\s*\(([a-zA-Z_][a-zA-Z0-9_]*)\)\s*;"
lua_regex="Native\.StartTimelineAction\s*\(([a-zA-Z_][a-zA-Z0-9_]*)\)\s*"
find_regex=".*\.\(lua\|h\|cpp\)"

function generate_labels {
    local dir=$1

    for f in $(find $dir -regex $find_regex)
    do
        while IFS= read -r line;
        do
        category=0
        if [[ $line =~ $lua_regex ]]
        then
            category=1
        elif [[ $line =~ $cpp_regex ]]
        then
            category=2
        fi

        if [ $category -gt 0 ]
        then
            val="${BASH_REMATCH[1]}"
            labels+=("$val,$category")
        fi
        linecount=$((linecount + 1))
        done < $f
    done

    if [ ${#labels[@]} -gt 0 ];
    then
	eval labels=($(printf "%q\n" "${labels[@]}" | sort -u))
    fi
}

function print_help {
    echo "Usage: ./generate_action_labels.sh [OPTION]..."
    echo "Generate action label helper files for hawktracer profiling"
    echo ""
    echo "Input control:"
    echo "  --dir=SEARCH_DIRECTORY      file search directory"
    echo "                              this parameter can be used multiple times in one command"
    echo "                              default: current working directory"
    echo ""
    echo "Regexp selection:"
    echo "  --find-regex=FIND_REGEX     regular expression used for finding files"
    echo "                              default: $find_regex"
    echo "  --cpp-regex=CPP_REGEX       regular expression used for finding tracepoint in C++ files"
    echo "                              default: $cpp_regex"
    echo "  --lua-regex=LUA_REGEX       regular expression used for finding tracepoint in LUA files"
    echo "                              default: $lua_regex"
    echo ""
    echo "Output control:"
    echo "  --cpp=CPP_HEADER_FILE       output C++ header file"
    echo "  --lua                       prints lua variables to standard output"
    echo "  --map=MAP_FILE              output map file used for profiler client"

}

if [ "$#" -eq "0" ];
then
    print_help
    exit
fi

directories=()
dir_index=0

for i in "$@"
do
    case $i in
        --cpp=*)
            cpp_output_file="${i#*=}"
            ;;
        --lua)
            generate_lua_variables=true
            ;;
        --map=*)
	    map_output_file="${i#*=}"
            ;;
        --find-regex=*)
            find_regex="${i#*=}"
            ;;
        --cpp-regex=*)
            cpp_regex="${i#*=}"
            ;;
        --lua-regex=*)
            lua_regex="${i#*=}"
            ;;
        --dir=*)
            dir="${i#*=}"
	    directories[dir_index]=$dir
	    ((dir_index+=1))
            ;;
        *)
            print_help
            exit
            ;;
    esac
    shift
done

function join_by { local IFS="$1"; shift; echo "$*"; }
directories_str=$(join_by " " "${directories[@]}")
generate_labels "$directories_str"

if [ -n "$cpp_output_file" ];
then
    generate_cpp_header $cpp_output_file labels
fi
if [ -n "$generate_lua_variables" ];
then
    generate_lua_variables labels ${#labels[@]}
fi
if [ -n "$map_output_file" ];
then
    generate_map $map_output_file labels
fi
