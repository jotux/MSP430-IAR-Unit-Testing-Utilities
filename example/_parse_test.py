import re
import sys

num_tests = -1
num_fails = -1

def main():
    log_file = open('_test.txt', 'r')
    for line in log_file:
        if re.search('Tests run: \d+',line):
            num_tests = int(line.split(':')[-1])
        if re.search('Tests Failed: \d+',line):
            num_fails = int(line.split(':')[-1])
    log_file.close()
    WriteUnitTestXml()
    if (num_fails > 0):
        sys.exit(1)
    else:
        sys.exit(0)

def WriteUnitTestXml():
    num_suites = 0
    cur_class = "null"
    test_file = open('_test.xml','w')
    log_file = open('_test.txt', 'r')
    test_file.write("<testsuite name=\"Tests\">\n")
    for line in log_file:
        if re.search('^\w+ Suite',line):
            cur_class = line.split(' ')[0]
        if re.search('^\w+ - test_',line):
            test_time = re.search('\|time:(.*)\|',line).group(1)
            pass_fail = line.split(' ')[0]
            test_name = re.search('test_(.*)\)',line).group()
            test_file.write("    <testcase classname=\"" + cur_class + "\" " + "name=\"" + test_name + "\" " + "time=\"" + test_time + "\">\n")
            if pass_fail == "FAIL":
                fail_message = re.search('\)-(.*?)(?=\|)',line).group()[2:]
                test_file.write("        <failure type=\"Fail\">" + fail_message + "</failure>\n")
            test_file.write("    </testcase>\n")
    test_file.write("</testsuite>")

if __name__ == "__main__":
    main()

#<testsuite name="name">
#    <testcase classname="foo" name="ASuccessfulTest"/>
#    <testcase classname="foo" name="AnotherSuccessfulTest"/>
#    <testcase classname="foo" name="AFailingTest">
#        <failure type="NotEnoughFoo"> details about failure </failure>
#    </testcase>
#</testsuite>