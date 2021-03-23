import os
import glob
import pandas as pd
import sys
os.chdir(".")

pattern = sys.argv[1]+"*.csv"
all_filenames = [i for i in glob.glob(pattern)]

#combine all files in the list
#combined_csv = pd.concat([pd.read_csv(f, index_col=0) for f in all_filenames ], axis=0, join='outer', ignore_index=False, sort=False)
#export to csv
combined_csv = pd.concat([pd.read_csv(f, index_col=0) for f in all_filenames ], sort=False, axis=1)
df = pd.DataFrame(combined_csv)
# df.sort('student', ascending=False)
df.to_csv( sys.argv[1]+".csv", index=True, encoding='utf-8-sig')

