import random

with open("seq", "w") as file:
    for i in range(1000000):
        num = i<<6
        hex_string = hex(num)[2:]
        nonmemops = random.randint(1, 300)
        file.write(str(nonmemops) + " R 0x" + hex_string + " 0x1" + "\n")
    file.close()



