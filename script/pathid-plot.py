import matplotlib.pyplot as plt
import os

# Function to plot data from a file
def plot_data(file_path):
    with open(file_path, 'r') as file:
        data = file.readlines()[:10000]  # Limit to the first 100,000 lines
        # data = file.readlines()
        data = [int(line.strip()) for line in data]

    plt.figure(figsize=(8, 6))
    plt.scatter(range(1, len(data) + 1), data, color='blue', marker='o', s=1)
    plt.xlabel('Index')
    plt.ylabel('Value')
    plt.title(f'Plot for {file_path}')
    plt.grid(True)
    plt.tight_layout()

    file_name = os.path.basename(file_path)
    output_file = os.path.splitext(file_name)[0] + '.png'
    plt.savefig(output_file)
    plt.show()

file_list = [file for file in os.listdir() if 'pathID' in file and file.endswith('.txt')]

# Plot each file and save the plot as XX.png
for file_path in file_list:
    plot_data(file_path)
