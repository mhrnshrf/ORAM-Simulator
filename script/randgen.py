import random

with open("rand846", "w") as file:
    for i in range(12514718):
        num = random.randint(1, 846811)
        num = num<<6
        hex_string = hex(num)[2:]
        nonmemops = random.randint(1, 300)
        file.write(str(nonmemops) + " R 0x" + hex_string + " 0x1" + "\n")
    file.close()



