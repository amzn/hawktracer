import argparse
import os
import re
import sys


class LanguageSupportArgs:
    _args = None

    def __init__(self, arg_parser, lang_id, lang_full, search_file_regex, search_tp_regex):
        self._lang_id = lang_id

        self._search_dir_opt = 'search-{}-dir'.format(self._lang_id)
        self._out_file_opt = 'out-{}-file'.format(self._lang_id)
        self._search_regex_opt = 'search-{}-regex'.format(self._lang_id)
        self._regex_opt = '{}-regex'.format(self._lang_id)

        arg_parser.add_argument('--{}'.format(self._search_dir_opt),
                                help='Search directory for {} language'.format(lang_full),
                                default=os.getcwd())
        arg_parser.add_argument('--{}'.format(self._out_file_opt),
                                help='An output {} file containing labels definitions.'.format(lang_full))
        arg_parser.add_argument('--{}'.format(self._search_regex_opt),
                                help='Search regular expression for {} files'.format(lang_full),
                                default=search_file_regex)
        arg_parser.add_argument('--{}'.format(self._regex_opt),
                                help='Regular expression for finding labels in {} files'.format(lang_full),
                                default=search_tp_regex)

    def set_parser_args(self, parser_args):
        self._args = vars(parser_args)

    def _get_param(self, opt):
        return self._args[opt.replace('-', '_')]

    def get_search_dir(self):
        return self._get_param(self._search_dir_opt)

    def get_out_file(self):
        return self._get_param(self._out_file_opt)

    def get_search_regex(self):
        return self._get_param(self._search_regex_opt)

    def get_regex(self):
        return self._get_param(self._regex_opt)

    def get_language_id(self):
        return self._lang_id


class LanguageSupport:
    _output_callback = None
    _labels = None
    _parameters = None
    _start_label_id = 1

    def __init__(self, parameters, output_callback):
        self._parameters = parameters
        self._output_callback = output_callback

    def get_parameters(self):
        return self._parameters

    def set_start_label_id(self, start_label_id):
        self._start_label_id = start_label_id

    def get_start_label_id(self):
        return self._start_label_id

    def get_labels(self):
        if self._labels is not None:
            return self._labels

        self._labels = []
        label_set = set()

        if not self._parameters.get_out_file():
            return self._labels

        file_regex = re.compile(self._parameters.get_search_regex())

        label_regex = re.compile(self._parameters.get_regex())

        for root, _, file_names in os.walk(self._parameters.get_search_dir()):
            file_paths = [os.path.abspath(os.path.join(root, file_path)) for file_path in file_names]
            for file_path in filter(file_regex.match, file_paths):
                for label_match in self._get_matches(file_path, label_regex):
                    if label_match.group(1) not in label_set:
                        self._labels.append((label_match.group(1), self._start_label_id))  # todo group nr to arguments
                        self._start_label_id += 1
                        label_set.add(label_match.group(1))

        return sorted(self._labels)

    def write_labels(self, stream):
        self._output_callback(self.get_labels(), stream)

    @staticmethod
    def _get_matches(file_name, regex):
        return [regex.match(line) for line in open(file_name) if regex.match(line)]


def generate_c_cpp_header(cpp_labels, stream):
    stream.write('#ifndef HAWKTRACER_AUTOGEN_TRACEPOINT_LABELS_H\n')
    stream.write('#define HAWKTRACER_AUTOGEN_TRACEPOINT_LABELS_H\n\n')

    for label in cpp_labels:
        stream.write('#define hawktracer_autogen_tracepoint_label_{} {}\n'.format(label[0], label[1]))

    stream.write('\n#endif /* HAWKTRACER_AUTOGEN_TRACEPOINT_LABELS_H */\n')


def generate_lua_file(lua_labels, stream):
    stream.write('local {} = {}\n'.format(
        ','.join([l[0] for l in lua_labels]), ','.join([str(l[1]) for l in lua_labels])))


def write_labels(lang_support):
    if lang_support.get_parameters().get_out_file():
        with open(lang_support.get_parameters().get_out_file(), 'w') as f:
            lang_support.write_labels(f)


def write_map(stream, labels_map):
    for lang_id in labels_map:
        for label in labels_map[lang_id]:
            stream.write('{} {} {}\n'.format(lang_id, label[0], label[1]))


def run_program(argv):
    parser = argparse.ArgumentParser(description='Generates tracepoint labels and map file.')
    parser.add_argument('--output-map', help='Output map file')

    languages = [
        LanguageSupport(
            LanguageSupportArgs(parser, 'cpp', 'C/C++', '.*\.(c|cpp|h|hpp)',
                                '^\s*HT_TP_GLOBAL_SCOPED_INT\s*\(([_A-Za-z][A-Za-z_0-9]+)\)\s*;\s*$'),
            generate_c_cpp_header),
        LanguageSupport(
            LanguageSupportArgs(parser, 'lua', 'LUA', '.*\.lua',
                                '^\s*--@TracepointStart\s+([_A-Za-z][A-Za-z_0-9]+)\s*$'),
            generate_lua_file)
    ]

    args = parser.parse_args(argv)

    labels_dict = {}

    start_label_id = 1
    for lang in languages:
        lang.get_parameters().set_parser_args(args)
        lang.set_start_label_id(start_label_id)
        write_labels(lang)
        labels_dict[lang.get_parameters().get_language_id()] = lang.get_labels()
        start_label_id = lang.get_start_label_id()

    if args.output_map:
        with open(args.output_map, 'w') as f:
            write_map(f, labels_dict)


if __name__ == '__main__':
    run_program(sys.argv[1:])
