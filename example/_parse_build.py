import re
import sys

num_error = -1
num_warn = -1

def main():
    log_file = open('_build.txt', 'r')
    for line in log_file:
        if re.search('Total number of errors: \d+',line):
            num_error = int(line.split(':')[-1])
        if re.search('Total number of warnings: \d+',line):
            num_warn = int(line.split(':')[-1])
    if (num_error > 0):
        print "Build fail"
        sys.exit(1)
    else:
        print "Build Success"
        sys.exit(0)

if __name__ == "__main__":
    main()