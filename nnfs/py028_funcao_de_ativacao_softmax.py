import numpy as np

y_pred = np.random.randn(3,3)

max_y = np.max(y_pred, axis=1, keepdims=True)

print(f"y_pred:\n{y_pred}\n")

print(f"max_y:\n{max_y}\n")

y_pred_normalized = y_pred - max_y

print(f"y_pred_normalized:\n{y_pred_normalized}\n")

exp_values = np.exp(y_pred_normalized)

print(f"exp_values:\n{exp_values}\n")

exp_sums = np.sum(exp_values, axis=1)

