geometry = open('geometry', 'r').readlines()
img = open('raw-output-00004-a', 'r').readlines()

n = 1024 * 1024

print(7 * n)

for i in range(n):
    print(img[i].strip(), geometry[i].strip())