from subprocess import Popen
import sys
Popen("./batch.sh " + sys.argv[1], shell=True).wait()
Popen("./gather.sh " + sys.argv[1], shell=True).wait()
print("Done!")
