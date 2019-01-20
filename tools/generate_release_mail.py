import sys
import subprocess
import re


class AuthorInfo:
    added_lines = 0
    removed_lines = 0
    changed_files = 0
    name = ''
    email = ''
    commit_count = 0

    def __init__(self, added_lines, removed_lines, changed_files, name, email, commit_count):
        self.added_lines = added_lines
        self.removed_lines = removed_lines
        self.changed_files = changed_files
        self.name = name
        self.email = email
        self.commit_count = commit_count


class GitInfo:
    _from_version = ''
    _to_version = ''

    def __init__(self, from_version, to_version):
        self._from_version = from_version
        self._to_version = to_version

    def get_summary_stats(self):
        return subprocess.getoutput('git diff {}..{} --shortstat'.format(self._from_version, self._to_version)).strip()

    def get_number_of_commits(self):
        return len(subprocess.getoutput('git log {}..{} --oneline'
                                        .format(self._from_version, self._to_version)).splitlines())

    def _get_author_stats(self, author_name, author_email, commit_count):
        stats = subprocess.getoutput(
            'git log {}..{} --numstat --pretty="" --author="{} <{}>"'.format(
                self._from_version, self._to_version, author_name, author_email)).splitlines()

        added_lines = 0
        removed_lines = 0
        for stat_line in stats:
            try:
                added_lines = added_lines + int(stat_line.split()[0])
                removed_lines = removed_lines + int(stat_line.split()[1])
            except ValueError:
                pass

        return AuthorInfo(added_lines, removed_lines, len(stats), author_name, author_email, commit_count)

    def get_list_of_authors(self):
        author_regex = re.compile('(\d+)\s+(.+)\s+<(.+)>')
        git_authors = subprocess.getoutput('git shortlog {}..{} -sne'
                                           .format(self._from_version, self._to_version)).splitlines()
        return [self._get_author_stats(m.group(2), m.group(3), m.group(1))
                for l in git_authors for m in [author_regex.search(l)] if m]

    @staticmethod
    def get_most_recent_tags():
        tag_regex = re.compile('tag:\s(v[a-zA-Z\d.]+)')
        git_tags = subprocess.getoutput('git log  --tags --simplify-by-decoration --pretty="format:%D"').splitlines()
        return [m.group(1) for l in git_tags for m in [tag_regex.search(l)] if m][0:2]

    @staticmethod
    def is_master_branch():
        return subprocess.getoutput('git rev-parse --abbrev-ref HEAD').strip() == 'master'


if len(sys.argv) < 2:
    print('Usage: {} <codename>'.format(sys.argv[0]))
    sys.exit(1)

if not GitInfo.is_master_branch():
    print('!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!')
    print('!!! YOU ARE NOT IN MASTER BRANCH !!!')
    print('!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!')
    print('')

tags = GitInfo.get_most_recent_tags()

if len(tags) < 2:
    print('Expected to find at least 2 git tags, but found {}'.format(len(tags)))
    sys.exit(1)

current_tag = tags[0]
current_version = current_tag[1:]
previous_tag = tags[1]
previous_version = previous_tag[1:]
code_name = sys.argv[1]

git_info = GitInfo(previous_tag, current_tag)

print("I'm pleased to announce HawkTracer {} ({}) release!".format(current_version, code_name))
print("URL: https://github.com/loganek/hawktracer/releases/tag/{}".format(current_tag))
print("Changes")
print("==========================================")
found_version = False
with open("NEWS") as f:
    for line in f:
        if line.startswith(current_version):
            found_version = True
        elif line.startswith(previous_version):
            break
        elif found_version:
            sys.stdout.write(line)

authors = git_info.get_list_of_authors()

print("Git statistics")
print("==========================================")
print("  {}".format(git_info.get_summary_stats()))
print("  Number of commits: {}".format(git_info.get_number_of_commits()))
print("  Contributors: {}".format(len(authors)))
for author in authors:
    alignment = len(authors[0].commit_count) - len(author.commit_count)
    print("    * {}{} {} <{}> {} files changed (+{}, -{})".format(
        ' ' * alignment, author.commit_count, author.name, author.email,
        author.changed_files, author.added_lines, author.removed_lines))
