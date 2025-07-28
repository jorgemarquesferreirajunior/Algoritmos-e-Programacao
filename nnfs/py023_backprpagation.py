import numpy as np
import matplotlib.pyplot as plt

inputs = np.array([1., -2., 3.])
weights = np.array([-3., -1., 2.])
bias = 1.0

y_true = 0.
lr = .001


def relu(x):
    return np.maximum(0, x)

def drelu(x):
    return np.where(x > 0, 1.0, 0.)

print(f"Initial weights:\n{weights}")
print(f"Initial bias:\n{bias}")

losses = []
for iteration in range(2):
    sum_weightsXinputs = np.dot(weights, inputs) + bias
    relu_output = relu(sum_weightsXinputs)
    loss = (relu_output - y_true) ** 2
    losses.append(loss)

    dloss_drelu_output = 2 * (relu_output - y_true)
    drelu_output_dsum = drelu(sum_weightsXinputs)
    dsum_dweights = inputs
    dsum_dbias = 1.0

    dloss_drelu = dloss_drelu_output * drelu_output_dsum
    dloss_dweights = dloss_drelu * dsum_dweights
    dloss_dbias = dloss_drelu * dsum_dbias

    weights -= lr * dloss_dweights
    bias -= lr * dloss_dbias

    # if iteration % 20 == 0: print(f"iteration: {iteration}, Loss: {loss}")
    print(f"iteration: {iteration}, Loss: {loss}")

print(f"Final weights:\n{weights}")
print(f"Final bias:\n{bias}")

plt.plot(losses)
plt.xlabel("Iteração")
plt.ylabel("Perda")
plt.title("Convergência da perda")
plt.grid(True)
plt.show()
  