import argparse
import copy
import re
import shutil
import sys
import os


class ConditionalFeature:
    def __init__(self, header_macro, custom_source_macro, header, files):
        self._header_macro = header_macro
        self._custom_source_macro = custom_source_macro
        self._files = files
        self._header = header

    def print_files(self, lib_dir, file_dumper):
        file_dumper.get_handle().write(
            '#if defined({}) && defined({})\n'.format(self._header_macro, self._custom_source_macro))
        for entry in self._files:
            file_dumper.get_handle().write('#elif defined({}) && ({})\n'.format(self._header_macro, entry.condition))
            file_dumper.dump_file(os.path.join(lib_dir, entry.source_path))
        file_dumper.get_handle().write("#endif\n")

    def is_using_file(self, file_path):
        for f in self._files:
            if f.source_path == file_path:
                return True

        return False


class ConditionalSource:
    def __init__(self, source_path, condition):
        self.source_path = source_path
        self.condition = condition


skip_headers = ['hawktracer/event_macros_impl.h']

platform_features = [
    ConditionalFeature(
        'HT_PLATFORM_FEATURE_CPU_USAGE_ENABLED', 'HT_PLATFORM_FEATURE_CPU_USAGE_CUSTOM_SOURCE',
        'hawktracer/cpu_usage.h',
        [ConditionalSource('platform/linux/cpu_usage.c', "defined(__unix__)")]),
    ConditionalFeature(
        'HT_PLATFORM_FEATURE_MEMORY_USAGE_ENABLED', 'HT_PLATFORM_FEATURE_MEMORY_USAGE_CUSTOM_SOURCE',
        'hawktracer/memory_usage.h',
        [
            ConditionalSource('platform/linux/memory_usage.c', "defined(__unix__)"),
            ConditionalSource('platform/windows/memory_usage.c', "defined(_WIN32)")
        ]),
    ConditionalFeature(
        'HT_PLATFORM_FEATURE_ALLOC_HOOKS_ENABLED', 'HT_PLATFORM_FEATURE_ALLOC_HOOKS_CUSTOM_SOURCE',
        'hawktracer/alloc_hooks.h',
        [ConditionalSource('platform/linux/alloc_hooks.c', "defined(__unix__)")])
]


def is_platform_feature(file_path):
    for feature in platform_features:
        if feature.is_using_file(file_path):
            return True
    return False


def load_file_list(root_dir, extensions, path_prefix, current_path=''):
    file_list = []
    for entry in os.scandir(os.path.join(root_dir, current_path)):
        if entry.is_dir():
            file_list += load_file_list(root_dir, extensions, path_prefix, os.path.join(current_path, entry.name))
        else:
            ext = os.path.splitext(entry.name)[-1].lower()
            if ext in extensions:
                file_list.append(os.path.join(path_prefix, current_path, entry.name))
    return file_list


def get_header(line):
    header_regex = re.compile(r'#\s*include\s+[<"](?P<path>.*?)[">]')

    m = header_regex.match(line)
    if not m or 1 != len(m.groups()):
        return None
    else:
        return m.groups()[0]


class Graph:
    def __init__(self, nodes):
        self._nodes = {}

        for node in nodes:
            if node not in self._nodes:
                self._nodes[node] = []

    def _get_free_node(self):
        for key, value in self._nodes.items():
            if len(value) == 0:
                return key

        return None

    def add_edge(self, src_node, dest_node):
        if src_node not in self._nodes[dest_node]:
            self._nodes[dest_node].append(src_node)

    def topology_sort(self):
        sorted_nodes = []
        init_nodes = copy.deepcopy(self._nodes)

        while len(self._nodes):
            node = self._get_free_node()
            assert node
            sorted_nodes.append(node)
            self.remove_node(node)

        self._nodes = init_nodes
        return sorted_nodes

    def remove_node(self, node):
        del self._nodes[node]
        for key, value in self._nodes.items():
            if node in value:
                value.remove(node)


class HeadersDB:
    _header_extensions = ('.h', '.hpp')
    config_header = 'hawktracer/ht_config.h'

    def __init__(self, lib_dir):
        self._include_dir = os.path.join(lib_dir, 'include')
        self._public_headers = load_file_list(os.path.join(self._include_dir, 'hawktracer'), self._header_extensions,
                                              'hawktracer') + [self.config_header]
        self._internal_headers = load_file_list(os.path.join(self._include_dir, 'internal'), self._header_extensions,
                                                'internal')

    def is_hawktracer_header(self, header_path):
        return header_path in self._public_headers or header_path in self._internal_headers

    def _get_headers_from_file(self, header_file):
        if header_file == self.config_header:
            return []

        with open(os.path.join(self._include_dir, header_file)) as f:
            for line in f:
                header = get_header(line)
                if header:
                    yield header

    def _get_header_graph(self, headers):
        g = Graph(headers)
        for header in headers:
            for dependency in self._get_headers_from_file(header):
                if dependency in headers:
                    g.add_edge(dependency, header)
        return g

    def get_internal_header_graph(self):
        return self._get_header_graph(self._internal_headers)

    def get_public_header_graph(self):
        return self._get_header_graph(self._public_headers)


class FileDumper:
    def __init__(self, file_handle, headers_db):
        self._file_handle = file_handle
        self._headers_db = headers_db

    @staticmethod
    def _is_header_guard(line):
        base_pattern = "\\s+HAWKTRACER_[A-Z_0-9]+_(H|HPP)\\s*"

        return re.match("^\s*#ifndef{}$".format(base_pattern), line, re.M) or re.match(
           "^\s*#define{}$".format(base_pattern), line, re.M) or re.match(
           "^\s*#endif\s*/\*{}\*/\s*$".format(base_pattern), line, re.M)

    def _is_line_ok_to_write(self, line):
        if FileDumper._is_header_guard(line):
            return False

        header = get_header(line)
        if not header:
            return True

        return not self._headers_db.is_hawktracer_header(header)

    def get_handle(self):
        return self._file_handle


class HeaderFileDumper(FileDumper):
    def __init__(self, file_handle, headers_db, include_dir):
        FileDumper.__init__(self, file_handle, headers_db)
        self._include_dir = include_dir

    def dump_file(self, in_file_name):
        if in_file_name == HeadersDB.config_header:
            self._file_handle.write("#include \"ht_config.h\"\n")
            return

        with open(os.path.join(self._include_dir, in_file_name)) as f:
            for line in f:
                if self._is_line_ok_to_write(line):
                    self._file_handle.write(line)


class SourceFileDumper(FileDumper):
    def __init__(self, file_handle, headers_db):
        FileDumper.__init__(self, file_handle, headers_db)

    def dump_file(self, in_file_name):
        self._file_handle.write("/** <BEGIN> File {} **/\n".format(in_file_name))
        with open(in_file_name) as f:
            for line in f:
                if self._is_line_ok_to_write(line):
                    self._file_handle.write(line)
        self._file_handle.write("/** <END> File {} **/\n\n".format(in_file_name))


class Amalgamator:
    _source_extensions = ('.c', '.cpp')

    def __init__(self, root_dir, out_header, out_source):
        self._header_list = []
        self._lib_dir = os.path.join(root_dir, 'lib')
        self._license_file = os.path.join(root_dir, "LICENSE")
        self._include_dir = os.path.join(self._lib_dir, 'include')
        self._source_list = load_file_list(self._lib_dir, self._source_extensions, '')
        self._out_header = out_header
        self._out_source = out_source

        self._headers = HeadersDB(self._lib_dir)

    def _dump_license(self, file_handle):
        with open(self._license_file, 'r') as license_file:
            file_handle.write("/************************************\n")
            for line in license_file:
                file_handle.write(" * " + line)
            file_handle.write(" ***********************************/\n\n")

    def _generate_header_file(self):
        print("Generate header file " + self._out_header)
        with open(self._out_header, "w") as f:
            header_file_dumper = HeaderFileDumper(f, self._headers, self._include_dir)

            self._dump_license(f)

            header_guard = "HAWKTRACER_ALL_H"
            f.write("#ifndef {}\n".format(header_guard))
            f.write("#define {}\n\n".format(header_guard))

            for header in self._headers.get_public_header_graph().topology_sort():
                header_file_dumper.dump_file(header)

            f.write("#endif /* {} */\n".format(header_guard))

    def _generate_source_file(self):
        print("Generate source file " + self._out_source)
        with open(self._out_source, "w") as f:
            source_file_dumper = SourceFileDumper(f, self._headers)
            header_file_dumper = HeaderFileDumper(f, self._headers, self._include_dir)

            self._dump_license(f)

            f.write("#define MKCREFLECT_IMPL /* define events declared in header */\n")
            header_file_dumper.dump_file("hawktracer/event_macros_impl.h")
            f.write("#include \"hawktracer.h\"\n\n")

            for header in self._headers.get_internal_header_graph().topology_sort():
                header_file_dumper.dump_file(header)

            for source_file in self._source_list:
                if not is_platform_feature(source_file):
                    source_file_dumper.dump_file(os.path.join(self._lib_dir, source_file))

            for feature in platform_features:
                feature.print_files(self._lib_dir, source_file_dumper)

    def generate(self):
        print("Generating files...")
        self._generate_header_file()
        self._generate_source_file()
        print("Generating files completed successfully!")


def run_program(argv):
    parser = argparse.ArgumentParser(description='Generate a single amalgamated source/header file for HawkTracer '
                                                 'library.')
    parser.add_argument('--root-dir', help='Project root directory', required=True)
    parser.add_argument('--out-dir', help='Output directory', default='.')
    parser.add_argument('--config-path', help='Path to a ht_config.h file', required=True)
    parser.add_argument('--cpp', help='Generate C++ source file', action='store_true')

    args = parser.parse_args(argv)

    os.makedirs(args.out_dir, exist_ok=True)
    shutil.copyfile(args.config_path, os.path.join(args.out_dir, "ht_config.h"))

    header_file = os.path.join(args.out_dir, 'hawktracer.h')
    source_file = os.path.join(args.out_dir, 'hawktracer.' + ('cpp' if args.cpp else 'c'))

    Amalgamator(args.root_dir, header_file, source_file).generate()


if __name__ == '__main__':
    run_program(sys.argv[1:])
