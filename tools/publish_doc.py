import argparse
import distutils.dir_util
import os
import subprocess
import sys
import tempfile


def call_process(call_args, custom_error, exit_on_error=False):
    try:
        subprocess.check_output(call_args)
    except subprocess.CalledProcessError as e:
        print(e.output)
        print('{}. Return code: {}'.format(custom_error, e.returncode))
        if exit_on_error:
            sys.exit(1)
        else:
            raise


parser = argparse.ArgumentParser(description='Publishes HawkTracer documentation')
parser.add_argument('-b', '--build-dir', help='HawkTracer build directory', required=True)
parser.add_argument('-r', '--repository', help='HawkTracer repository path',
                    default='git@github.com:amzn/hawktracer.git')
args = parser.parse_args()

call_process(['cmake', '--build', args.build_dir, '--target', 'doc_doxygen'], 'Building documentation failed.', True)

doc_path = os.path.abspath(os.path.join(args.build_dir, 'doxygen_doc', 'html'))
if not os.path.exists(doc_path):
    print('Documentation path "{}" doesn\'t exist!'.format(doc_path))
    sys.exit(1)

current_dir = os.getcwd()
clone_dir = tempfile.mkdtemp()

try:
    call_process(['git', 'clone', args.repository, clone_dir], 'Unable to clone repository')
    os.chdir(clone_dir)
    call_process(['git', 'checkout', 'gh-pages'], 'Unable to switch to doc branch.')

    print('Removing old documentation...')
    call_process(['git', 'rm', '-rfq', '*'], 'Unable to remove previous documentation')

    print('Copying documentation to a repository...')
    distutils.dir_util.copy_tree(doc_path, clone_dir)

    print("Committing documentation...")
    call_process(['git', 'add', '.'], 'Unable to add files to commit.')
    call_process(['git', 'commit', '-a', '-m', 'update doc'], 'Unable to commit new documentation.')
    call_process(['git', 'push', 'origin', 'gh-pages'], 'Unable to push documentation.')

finally:
    distutils.dir_util.remove_tree(clone_dir)
