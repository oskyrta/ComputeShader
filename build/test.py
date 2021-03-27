import os

for i in range(10, 350, 10):
    os.system("cmd /c PhysicsCompute gpu {}".format(i))
