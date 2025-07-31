import numpy as np

inputs = np.random.randn(10,4)

def relu(inputs : np.ndarray) -> np.ndarray:
    return np.maximum(0, inputs)

relu_outputs = relu(inputs)

print(f"inputs:\n{inputs}\n")

print(f"relu_outputs:\n{relu_outputs}\n")

