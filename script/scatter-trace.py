import os
import matplotlib.pyplot as plt

# Get the list of files in the current directory
files = os.listdir()

# Iterate over each file and generate a scatter plot
for file in files:
    if file.endswith(".txt"):
        # Open the file and read the first 100000 lines
        with open(file, "r") as f:
            lines = f.readlines()[:100000]

        # Extract the first hex number from each line
        numbers = []
        for line in lines:
            parts = line.split()
            if len(parts) >= 3:
                hex_number = int(parts[2], 16)  # Convert hex to integer
                numbers.append(hex_number)

        # Define the x and y values for the scatter plot
        x_values = list(range(len(numbers)))
        y_values = numbers

        # Calculate the range of numbers in the file
        y_min = min(y_values)
        y_max = max(y_values)
        y_range = y_max - y_min
        print(y_range)

        # Create the scatter plot
        plt.scatter(x_values, y_values, s=0.5, alpha=0.5, color='black')

        # Set the plot title and labels for both axes
        file_name = file.replace("-raw.txt", "")
        plt.title(f"Scatter Plot of {file_name}")
        plt.xlabel("Access Timeline")
        plt.ylabel(file_name)

        # Set the x and y axis limits
        plt.xlim(0, len(numbers))
        plt.ylim(y_min - y_range * 0.1, y_max + y_range * 0.1)

        # Adjust the margins to make more space for the y-axis label
        plt.subplots_adjust(left=0.2)

        # Save the plot with a file name based on the original file name
        plot_name = os.path.splitext(file)[0] + ".png"
        plt.savefig(file_name + ".png")

        # Clear the plot for the next file
        plt.clf()
