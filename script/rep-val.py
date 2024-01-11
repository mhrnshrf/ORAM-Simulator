from collections import Counter
import os

# Function to count repeated values in a file
def count_repeated_values(file_path):
    with open(file_path, 'r') as file:
        data = [int(line.strip()) for line in file]

    value_counts = Counter(data)
    repeated_values = sum(count > 1 for count in value_counts.values())

    return repeated_values

# List all files in the directory with the pattern 'pathID-*.txt'
file_list = [file for file in os.listdir() if file.startswith('pathID-') and file.endswith('.txt')]

# Count repeated values in each file
for file_path in file_list:
    repeated_count = count_repeated_values(file_path)
    file_name = os.path.basename(file_path)
    print(f"In file '{file_name}', there are {repeated_count} values repeated more than once.")
