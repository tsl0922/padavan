#!/usr/bin/env python
#
# (C) 2012-2013 by Pablo Neira Ayuso <pablo@netfilter.org>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This software has been sponsored by Sophos Astaro <http://www.sophos.com>
#

from __future__ import print_function
import sys
import os
import subprocess
import argparse

IPTABLES = "iptables"
IP6TABLES = "ip6tables"
ARPTABLES = "arptables"
EBTABLES = "ebtables"

IPTABLES_SAVE = "iptables-save"
IP6TABLES_SAVE = "ip6tables-save"
ARPTABLES_SAVE = "arptables-save"
EBTABLES_SAVE = "ebtables-save"
#IPTABLES_SAVE = ['xtables-save','-4']
#IP6TABLES_SAVE = ['xtables-save','-6']

EXTENSIONS_PATH = "extensions"
LOGFILE="/tmp/iptables-test.log"
log_file = None

STDOUT_IS_TTY = sys.stdout.isatty()
STDERR_IS_TTY = sys.stderr.isatty()

def maybe_colored(color, text, isatty):
    terminal_sequences = {
        'green': '\033[92m',
        'red': '\033[91m',
    }

    return (
        terminal_sequences[color] + text + '\033[0m' if isatty else text
    )


def print_error(reason, filename=None, lineno=None):
    '''
    Prints an error with nice colors, indicating file and line number.
    '''
    print(filename + ": " + maybe_colored('red', "ERROR", STDERR_IS_TTY) +
        ": line %d (%s)" % (lineno, reason), file=sys.stderr)


def delete_rule(iptables, rule, filename, lineno):
    '''
    Removes an iptables rule
    '''
    cmd = iptables + " -D " + rule
    ret = execute_cmd(cmd, filename, lineno)
    if ret == 1:
        reason = "cannot delete: " + iptables + " -I " + rule
        print_error(reason, filename, lineno)
        return -1

    return 0


def run_test(iptables, rule, rule_save, res, filename, lineno, netns):
    '''
    Executes an unit test. Returns the output of delete_rule().

    Parameters:
    :param iptables: string with the iptables command to execute
    :param rule: string with iptables arguments for the rule to test
    :param rule_save: string to find the rule in the output of iptables-save
    :param res: expected result of the rule. Valid values: "OK", "FAIL"
    :param filename: name of the file tested (used for print_error purposes)
    :param lineno: line number being tested (used for print_error purposes)
    '''
    ret = 0

    cmd = iptables + " -A " + rule
    if netns:
            cmd = "ip netns exec ____iptables-container-test " + EXECUTABLE + " " + cmd

    ret = execute_cmd(cmd, filename, lineno)

    #
    # report failed test
    #
    if ret:
        if res != "FAIL":
            reason = "cannot load: " + cmd
            print_error(reason, filename, lineno)
            return -1
        else:
            # do not report this error
            return 0
    else:
        if res == "FAIL":
            reason = "should fail: " + cmd
            print_error(reason, filename, lineno)
            delete_rule(iptables, rule, filename, lineno)
            return -1

    matching = 0
    tokens = iptables.split(" ")
    if len(tokens) == 2:
        if tokens[1] == '-4':
            command = IPTABLES_SAVE
        elif tokens[1] == '-6':
            command = IP6TABLES_SAVE
    elif len(tokens) == 1:
        if tokens[0] == IPTABLES:
            command = IPTABLES_SAVE
        elif tokens[0] == IP6TABLES:
            command = IP6TABLES_SAVE
        elif tokens[0] == ARPTABLES:
            command = ARPTABLES_SAVE
        elif tokens[0] == EBTABLES:
            command = EBTABLES_SAVE

    command = EXECUTABLE + " " + command

    if netns:
            command = "ip netns exec ____iptables-container-test " + command

    args = tokens[1:]
    proc = subprocess.Popen(command, shell=True,
                            stdin=subprocess.PIPE,
                            stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    out, err = proc.communicate()

    #
    # check for segfaults
    #
    if proc.returncode == -11:
        reason = "iptables-save segfaults: " + cmd
        print_error(reason, filename, lineno)
        delete_rule(iptables, rule, filename, lineno)
        return -1

    # find the rule
    matching = out.find(rule_save.encode('utf-8'))
    if matching < 0:
        if res == "OK":
            reason = "cannot find: " + iptables + " -I " + rule
            print_error(reason, filename, lineno)
            delete_rule(iptables, rule, filename, lineno)
            return -1
        else:
            # do not report this error
            return 0
    else:
        if res != "OK":
            reason = "should not match: " + cmd
            print_error(reason, filename, lineno)
            delete_rule(iptables, rule, filename, lineno)
            return -1

    # Test "ip netns del NETNS" path with rules in place
    if netns:
        return 0

    return delete_rule(iptables, rule, filename, lineno)

def execute_cmd(cmd, filename, lineno):
    '''
    Executes a command, checking for segfaults and returning the command exit
    code.

    :param cmd: string with the command to be executed
    :param filename: name of the file tested (used for print_error purposes)
    :param lineno: line number being tested (used for print_error purposes)
    '''
    global log_file
    if cmd.startswith('iptables ') or cmd.startswith('ip6tables ') or cmd.startswith('ebtables ') or cmd.startswith('arptables '):
        cmd = EXECUTABLE + " " + cmd

    print("command: {}".format(cmd), file=log_file)
    ret = subprocess.call(cmd, shell=True, universal_newlines=True,
        stderr=subprocess.STDOUT, stdout=log_file)
    log_file.flush()

    # generic check for segfaults
    if ret == -11:
        reason = "command segfaults: " + cmd
        print_error(reason, filename, lineno)
    return ret


def variant_res(res, variant, alt_res=None):
    '''
    Adjust expected result with given variant

    If expected result is scoped to a variant, the other one yields a different
    result. Therefore map @res to itself if given variant is current, use the
    alternate result, @alt_res, if specified, invert @res otherwise.

    :param res: expected result from test spec ("OK", "FAIL" or "NOMATCH")
    :param variant: variant @res is scoped to by test spec ("NFT" or "LEGACY")
    :param alt_res: optional expected result for the alternate variant.
    '''
    variant_executable = {
        "NFT": "xtables-nft-multi",
        "LEGACY": "xtables-legacy-multi"
    }
    res_inverse = {
        "OK": "FAIL",
        "FAIL": "OK",
        "NOMATCH": "OK"
    }

    if variant_executable[variant] == EXECUTABLE:
        return res
    if alt_res is not None:
        return alt_res
    return res_inverse[res]


def run_test_file(filename, netns):
    '''
    Runs a test file

    :param filename: name of the file with the test rules
    '''
    #
    # if this is not a test file, skip.
    #
    if not filename.endswith(".t"):
        return 0, 0

    if "libipt_" in filename:
        iptables = IPTABLES
    elif "libip6t_" in filename:
        iptables = IP6TABLES
    elif "libxt_"  in filename:
        iptables = IPTABLES
    elif "libarpt_" in filename:
        # only supported with nf_tables backend
        if EXECUTABLE != "xtables-nft-multi":
           return 0, 0
        iptables = ARPTABLES
    elif "libebt_" in filename:
        # only supported with nf_tables backend
        if EXECUTABLE != "xtables-nft-multi":
           return 0, 0
        iptables = EBTABLES
    else:
        # default to iptables if not known prefix
        iptables = IPTABLES

    f = open(filename)

    tests = 0
    passed = 0
    table = ""
    chain_array = []
    total_test_passed = True

    if netns:
        execute_cmd("ip netns add ____iptables-container-test", filename, 0)

    for lineno, line in enumerate(f):
        if line[0] == "#" or len(line.strip()) == 0:
            continue

        if line[0] == ":":
            chain_array = line.rstrip()[1:].split(",")
            continue

        # external non-iptables invocation, executed as is.
        if line[0] == "@":
            external_cmd = line.rstrip()[1:]
            if netns:
                external_cmd = "ip netns exec ____iptables-container-test " + external_cmd
            execute_cmd(external_cmd, filename, lineno)
            continue

        # external iptables invocation, executed as is.
        if line[0] == "%":
            external_cmd = line.rstrip()[1:]
            if netns:
                external_cmd = "ip netns exec ____iptables-container-test " + EXECUTABLE + " " + external_cmd
            execute_cmd(external_cmd, filename, lineno)
            continue

        if line[0] == "*":
            table = line.rstrip()[1:]
            continue

        if len(chain_array) == 0:
            print_error("broken test, missing chain",
                        filename = filename, lineno = lineno)
            total_test_passed = False
            break

        test_passed = True
        tests += 1

        for chain in chain_array:
            item = line.split(";")
            if table == "":
                rule = chain + " " + item[0]
            else:
                rule = chain + " -t " + table + " " + item[0]

            if item[1] == "=":
                rule_save = chain + " " + item[0]
            else:
                rule_save = chain + " " + item[1]

            res = item[2].rstrip()
            if len(item) > 3:
                variant = item[3].rstrip()
                if len(item) > 4:
                    alt_res = item[4].rstrip()
                else:
                    alt_res = None
                res = variant_res(res, variant, alt_res)

            ret = run_test(iptables, rule, rule_save,
                           res, filename, lineno + 1, netns)

            if ret < 0:
                test_passed = False
                total_test_passed = False
                break

        if test_passed:
            passed += 1

    if netns:
        execute_cmd("ip netns del ____iptables-container-test", filename, 0)
    if total_test_passed:
        print(filename + ": " + maybe_colored('green', "OK", STDOUT_IS_TTY))

    f.close()
    return tests, passed


def show_missing():
    '''
    Show the list of missing test files
    '''
    file_list = os.listdir(EXTENSIONS_PATH)
    testfiles = [i for i in file_list if i.endswith('.t')]
    libfiles = [i for i in file_list
                if i.startswith('lib') and i.endswith('.c')]

    def test_name(x):
        return x[0:-2] + '.t'
    missing = [test_name(i) for i in libfiles
               if not test_name(i) in testfiles]

    print('\n'.join(missing))

def spawn_netns():
    # prefer unshare module
    try:
        import unshare
        unshare.unshare(unshare.CLONE_NEWNET)
        return True
    except:
        pass

    # sledgehammer style:
    # - call ourselves prefixed by 'unshare -n' if found
    # - pass extra --no-netns parameter to avoid another recursion
    try:
        import shutil

        unshare = shutil.which("unshare")
        if unshare is None:
            return False

        sys.argv.append("--no-netns")
        os.execv(unshare, [unshare, "-n", sys.executable] + sys.argv)
    except:
        pass

    return False

#
# main
#
def main():
    parser = argparse.ArgumentParser(description='Run iptables tests')
    parser.add_argument('filename', nargs='*',
                        metavar='path/to/file.t',
                        help='Run only this test')
    parser.add_argument('-H', '--host', action='store_true',
                        help='Run tests against installed binaries')
    parser.add_argument('-l', '--legacy', action='store_true',
                        help='Test iptables-legacy')
    parser.add_argument('-m', '--missing', action='store_true',
                        help='Check for missing tests')
    parser.add_argument('-n', '--nftables', action='store_true',
                        help='Test iptables-over-nftables')
    parser.add_argument('-N', '--netns', action='store_true',
                        help='Test netnamespace path')
    parser.add_argument('--no-netns', action='store_true',
                        help='Do not run testsuite in own network namespace')
    args = parser.parse_args()

    #
    # show list of missing test files
    #
    if args.missing:
        show_missing()
        return

    global EXECUTABLE
    EXECUTABLE = "xtables-legacy-multi"
    if args.nftables:
        EXECUTABLE = "xtables-nft-multi"

    if os.getuid() != 0:
        print("You need to be root to run this, sorry", file=sys.stderr)
        return

    if not args.netns and not args.no_netns and not spawn_netns():
        print("Cannot run in own namespace, connectivity might break",
              file=sys.stderr)

    if not args.host:
        os.putenv("XTABLES_LIBDIR", os.path.abspath(EXTENSIONS_PATH))
        os.putenv("PATH", "%s/iptables:%s" % (os.path.abspath(os.path.curdir),
                                              os.getenv("PATH")))

    test_files = 0
    tests = 0
    passed = 0

    # setup global var log file
    global log_file
    try:
        log_file = open(LOGFILE, 'w')
    except IOError:
        print("Couldn't open log file %s" % LOGFILE, file=sys.stderr)
        return

    if args.filename:
        file_list = args.filename
    else:
        file_list = [os.path.join(EXTENSIONS_PATH, i)
                     for i in os.listdir(EXTENSIONS_PATH)
                     if i.endswith('.t')]
        file_list.sort()

    for filename in file_list:
        file_tests, file_passed = run_test_file(filename, args.netns)
        if file_tests:
            tests += file_tests
            passed += file_passed
            test_files += 1

    print("%d test files, %d unit tests, %d passed" % (test_files, tests, passed))
    return passed - tests


if __name__ == '__main__':
    sys.exit(main())
