import math

for i in range(256):
    if(i % 10 == 0):
        print()
    x = i/255
    x = math.sqrt(x)
    x = x * 255
    print(int(x), end=", ")


