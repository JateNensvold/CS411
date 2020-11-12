import pandas as pd
import glob
import os

index = [1,2,4,8,16,32,64,128,256]
column = [1,2,4,8,16,32]
total_df = pd.DataFrame(index = index, columns = column)

directory = r'/home/alorphan/cs411/PA3/mpi_new_data'

# Grab all slurm files and turn them into dfs
for filename in os.listdir(directory):
    total_comm = 0
    total_run = 0
    processes = 0
    size = 0

    if filename.endswith(".out"):
        fullName = os.path.join(directory, filename)
        df = pd.read_csv(fullName, header=None)
        
        for index, row in df.iterrows():
            if(row[0] == 'comm' or row[0] == 'communication'):
                total_comm += row[1]
            elif (row[0] == 'runtime'):
                total_run = row[1]
            elif (row[0] == 'processes'):
                processes = row[1]
            elif (row[0] == 'size'):
                size = row[1]

        size = size/1000
        total_df.at[size, processes] = (total_run-total_comm)/processes

print(total_df)    
total_df.to_csv('mpi_data.csv')