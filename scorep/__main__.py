import os
import sys
import importlib
import getopt

import scorep.trace


global_trace = None


def main(argv=None):
    if argv is None:
        argv = sys.argv
    try:
        opts, prog_argv = getopt.getopt(argv[1:], "v",
                                        ["help", "version", "mpi"])

    except getopt.error as msg:
        sys.stderr.write("%s: %s\n" % (sys.argv[0], msg))
        sys.stderr.write("Try `%s --help' for more information\n"
                         % sys.argv[0])
        sys.exit(1)

    mpi = False

    for opt in opts:
        key, value = opt
        if key == "--help":
            sys.exit(0)

        if key == "--version":
            sys.stdout.write("scorep_trace 1.0\n")
            sys.exit(0)

        if key == "--mpi":
            mpi = True

    scorep_bindings = importlib.import_module("scorep.scorep_bindings")

    # everything is ready
    sys.argv = prog_argv
    progname = prog_argv[0]
    sys.path[0] = os.path.split(progname)[0]

    global_trace = scorep.trace.ScorepTrace(scorep_bindings, True)
    try:
        with open(progname) as fp:
            code = compile(fp.read(), progname, 'exec')
        # try to emulate __main__ namespace as much as possible
        globs = {
            '__file__': progname,
            '__name__': '__main__',
            '__package__': None,
            '__cached__': None,
        }
        global_trace.runctx(code, globs, globs)
    except OSError as err:
        print("Cannot run file {} because: {}".format(sys.argv[0], err))
    except SystemExit:
        pass


if __name__ == '__main__':
    main()

else:
    '''
    If Score-P is not intialised using the tracing module (`python -m scorep <script.py>`),
    we need to make sure that, if a user call gets called, scorep is still loaded.
    Moreover, if the module is loaded with `import scorep` we can't do any mpi support anymore
    '''
    scorep_bindings = importlib.import_module("scorep.scorep_bindings")
    global_trace = scorep.trace.ScorepTrace(scorep_bindings, False)
