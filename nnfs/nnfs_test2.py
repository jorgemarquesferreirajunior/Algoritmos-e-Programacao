from mynnfs import *
from nnfs.datasets import spiral_data
import nnfs

# entradas
nnfs.init()
inputs, y_true = spiral_data(samples=100, classes=3)

print(f"inputs{inputs.shape}[-10:]:\n{inputs[-10:]}\n")
print(f"y_true{y_true.shape}[-10:]:\n{y_true[-10:]}\n")
