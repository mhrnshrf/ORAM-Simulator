import os
import glob
import pandas as pd
import sys
os.chdir(".")

extension = 'csv'
all_filenames = [i for i in glob.glob(sys.argv[1]+'*.{}'.format(extension))]

#combine all files in the list
#combined_csv = pd.concat([pd.read_csv(f, index_col=0) for f in all_filenames ], axis=0, join='outer', ignore_index=False, sort=False)
#export to csv
combined_csv = pd.concat([pd.read_csv(f, index_col=0) for f in all_filenames ], sort=False, axis=1)
combined_csv.to_csv( sys.argv[1]+".csv", index=True, encoding='utf-8-sig')
