from subprocess import Popen
import sys
p = Popen("./batch.sh " + sys.argv[1], shell=True)
p.wait()
Popen("./gather.sh " + sys.argv[1], shell=True).wait()
print("Done!")
