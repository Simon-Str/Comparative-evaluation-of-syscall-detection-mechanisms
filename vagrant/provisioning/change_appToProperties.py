import json
import optparse


def isValidOpts():
    if not options.binary:
        parser.error("A binary name must be provided.")
        return False
    return True


def write_json(binary, filename='app.to.properties.json'):
    if options.debug: print("Filename: ", filename)
    with open(filename, 'r') as file:
        data = json.load(file)
        data["apps"][0][f"{binary}"] = {
            "enable": "true",
            "cfg": {
                "direct": f"{binary}.svf.conditional.direct.calls.cfg",
                "svf": f"{binary}.svf1-7.cfg",
                "svftype": f"{binary}.svf.type.cfg",
                "svftypefp": f"{binary}.svf.new.type.fp.wglobal.cfg"
            },
            "master": "main",
            "worker": "child_main",
            "bininput": f"{binary}/",
            "output": f"{binary}.syscall.out"
        }

    with open(filename, 'w') as file:
        json.dump(data, file, indent=4)
        updated_json_string = json.dumps(data, indent=4)
        print(updated_json_string)


if __name__ == '__main__':
    usage = "Usage: %prog -i </PATH/TO/app.to.properties.json -b <binaryname>"
    parser = optparse.OptionParser(usage=usage, version="1")
    parser.add_option("-i", "--input", dest="appToProperties", default=None, nargs=1,
                      help="Path to app.to.properties.json")
    parser.add_option("-d", "--debug", dest="debug", action="store_true", default=False,
                      help="Debug enabled/disabled")
    parser.add_option("-b", "--binary", dest="binary", default=None, nargs=1,
                      help="name of the binary")
    (options, args) = parser.parse_args()

    if isValidOpts():
        write_json(options.binary, options.appToProperties)
