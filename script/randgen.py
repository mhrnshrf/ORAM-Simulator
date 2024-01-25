import random

with open("randl24", "w") as file:
    for i in range(40000000):
        num = random.randint(1, 33554431)
        num = num<<6
        hex_string = hex(num)[2:]
        nonmemops = random.randint(1, 300)
        file.write(str(nonmemops) + " R 0x" + hex_string + " 0x1" + "\n")
    file.close()


# import random

# # Generate a random order of numbers between 1 to 1000000
# random_order = random.sample(range(1, 1000001), 1000000)

# with open("randonce", "w") as file:
#     for i in random_order:
#         num = (i - 1) << 6  # Adjusted to start from 0
#         hex_string = hex(num)[2:]
#         nonmemops = random.randint(1, 300)
#         file.write(str(nonmemops) + " R 0x" + hex_string + " 0x1" + "\n")




