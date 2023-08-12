import json
import optparse


def isValidOpts():
    if not options.appToProperties or not options.binary:
        parser.error("A path to the app.to.properties.json and a binary name must be provided.")
        return False
    return True


def write_json(new_data, filename='app.to.properties.json'):
    with open(filename, 'r+') as file:
        file_data = json.load(file)
        file_data["apps"].append(new_data)
        file.seek(0)
        json.dump(file_data, file, indent=4)
        updated_json_string = json.dumps(file_data, indent=4)
        print(updated_json_string)

def generate_json_object(binaryname):
    print(f"Der Binary name ist: {binaryname}")
    binJsonObject = {
        f"{binaryname}": {
            "enable": "true",
            "cfg": {
                "direct": f"{binaryname}.svf.conditional.direct.calls.cfg",
                "svf": f"{binaryname}.svf17.cfg",
                "svftype": f"{binaryname}.svf.type.cfg",
                "svftypefp": f"{binaryname}.svf.new.type.fp.wglobal.cfg"
            },
            "master": "main",
            "worker": "child_main",
            "bininput": f"{binaryname}/",
            "output": f"{binaryname}syscall.out"
        }
    }
    return binJsonObject


if __name__ == '__main__':
    usage = "Usage: %prog -i <BINARY.syscalls.out> -o <output file> -"

    parser = optparse.OptionParser(usage=usage, version="1")
    parser.add_option("-i", "--input", dest="appToProperties", default=None, nargs=1,
                      help="Path to app.to.properties.json")
    parser.add_option("-d", "--debug", dest="debug", action="store_true", default=False,
                      help="Debug enabled/disabled")
    parser.add_option("-b", "--output", dest="binary", default=None, nargs=1,
                      help="name of the binary")

    (options, args) = parser.parse_args()
    if isValidOpts():
        write_json(generate_json_object(options.binary))
