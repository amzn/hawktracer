import unittest
import os
import sys
import argparse
import filecmp

current_dir = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(current_dir, os.pardir, os.pardir, 'tools'))

import generate_tracepoint_labels

class InMemoryWriter:
    data = ''

    def write(self, text):
        self.data += text


class MockLanguageSupportArgs:
    def __init__(self, search_dir, out_file, search_regex, regex):
        self._search_dir = search_dir
        self._out_file = out_file
        self._search_regex = search_regex
        self._regex = regex

    def get_search_dir(self):
        return self._search_dir

    def get_out_file(self):
        return self._out_file

    def get_search_regex(self):
        return self._search_regex

    def get_regex(self):
        return self._regex


class TestLanguageArgsSupport(unittest.TestCase):
    def test_should_set_default_values(self):
        parser = argparse.ArgumentParser()

        args = generate_tracepoint_labels.LanguageSupportArgs(
            parser, 'lid', 'lfull', 'search-file-regex', 'search-tp-regex')

        args.set_parser_args(parser.parse_args())

        self.assertEqual('lid', args.get_language_id())
        self.assertEqual('search-file-regex', args.get_search_regex())
        self.assertEqual('search-tp-regex', args.get_regex())
        self.assertEqual(None, args.get_out_file())
        self.assertEqual(os.getcwd(), args.get_search_dir())


class TestLanguageSupport(unittest.TestCase):
    def test_cpp_simple(self):
        cpp_lang = generate_tracepoint_labels.LanguageSupport(
            MockLanguageSupportArgs(
                current_dir, 'out_file', '.*\.(c|cpp|h|hpp)',
                '^\s*HT_TP_GLOBAL_SCOPED_INT\s*\(([_A-Za-z][A-Za-z_0-9]+)\)\s*;\s*$'),
            generate_tracepoint_labels.generate_c_cpp_header)

        cpp_labels = cpp_lang.get_labels()

        self.assertEqual(2, len(cpp_labels))
        self.assertTrue(('ok_1', 1) in cpp_labels)
        self.assertTrue(('_ok_2', 2) in cpp_labels)

    def test_cpp_should_not_parse_labels_if_no_out_file(self):
        cpp_lang = generate_tracepoint_labels.LanguageSupport(
            MockLanguageSupportArgs(
                current_dir, None, '.*\.(c|cpp|h|hpp)',
                '^\s*HT_TP_GLOBAL_SCOPED_INT\s*\(([_A-Za-z][A-Za-z_0-9]+)\)\s*;\s*$'),
            generate_tracepoint_labels.generate_c_cpp_header)

        cpp_labels = cpp_lang.get_labels()

        self.assertEqual(0, len(cpp_labels))

    def test_write_callback(self):
        def callback(labels, stream):
            stream.write('this-is-test: {}'.format(len(labels)))

        general_lang = generate_tracepoint_labels.LanguageSupport(
            MockLanguageSupportArgs(
                current_dir, 'test', 'nonexistingpattern',
                'invalidpattern'), callback)
        general_lang._labels = [1, 2, 3]

        writer = InMemoryWriter()
        general_lang.write_labels(writer)

        self.assertEqual('this-is-test: 3', writer.data)

    def test_cpp_generator(self):
        writer = InMemoryWriter()

        test_labels = [('a', 1), ('bb', 3), ('ccc', 7)]
        generate_tracepoint_labels.generate_c_cpp_header(test_labels, writer)

        self.assertEqual('#ifndef HAWKTRACER_AUTOGEN_TRACEPOINT_LABELS_H\n'
                         '#define HAWKTRACER_AUTOGEN_TRACEPOINT_LABELS_H\n\n'
                         '#define hawktracer_autogen_tracepoint_label_a 1\n'
                         '#define hawktracer_autogen_tracepoint_label_bb 3\n'
                         '#define hawktracer_autogen_tracepoint_label_ccc 7\n'
                         '\n#endif /* HAWKTRACER_AUTOGEN_TRACEPOINT_LABELS_H */\n',
                         writer.data)

    def test_lua_generator(self):
        writer = InMemoryWriter()

        test_labels = [('a', 1), ('bb', 3), ('ccc', 7)]
        generate_tracepoint_labels.generate_lua_file(test_labels, writer)

        self.assertEqual('local a,bb,ccc = 1,3,7\n', writer.data)

    def test_write_map(self):
        writer = InMemoryWriter()
        test_map = {
            'lang1': [('lang1_label1', 5), ('lang1_label2', 2)],
            'lang2': [('lang2_label1', 5), ('lang2_label2', 2)]
        }

        generate_tracepoint_labels.write_map(writer, test_map)
        self.assertEqual('lang1 lang1_label1 5\nlang1 lang1_label2 2\n'
                         'lang2 lang2_label1 5\nlang2 lang2_label2 2\n', writer.data)

    def test_run_full_program(self):
        assets_dir = os.path.join(current_dir, 'assets')
        cpp_out = os.path.join(current_dir, 'cpp_out.tmp')
        lua_out = os.path.join(current_dir, 'lua_out.tmp')
        mapping_out = os.path.join(current_dir, 'mapping_out.tmp')

        generate_tracepoint_labels.run_program([
            '--out-cpp-file', cpp_out, '--search-cpp-dir', assets_dir,
            '--out-lua-file', lua_out, '--search-lua-dir', assets_dir,
            '--output-map', mapping_out
        ])

        self.assertTrue(filecmp.cmp(lua_out, os.path.join(assets_dir, 'goldenlua.out')))
        self.assertTrue(filecmp.cmp(cpp_out, os.path.join(assets_dir, 'goldencpp.out')))
        self.assertTrue(filecmp.cmp(mapping_out, os.path.join(assets_dir, 'goldenmapping.out')))

        os.remove(cpp_out)
        os.remove(lua_out)
        os.remove(mapping_out)

if __name__ == '__main__':
    unittest.main()
