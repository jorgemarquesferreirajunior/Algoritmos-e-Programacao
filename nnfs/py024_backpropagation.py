import numpy as np

x = np.array([1.,2.,3.,4.])
w = np.array([
    [.1, .2, .3, .4],
    [.5, .6, .7, .8],
    [.9, 1., 1.1, 1.2],
])
b = np.array([.1, .2, .3])

print("\nPROPAGATION\n")

z = np.dot(x, w.T) + b
print(f"z:\n{z}\n")

a = np.maximum(0, z)
print(f"a:\n{a}\n")

y = np.sum(a)
print(f"y:\n{y}\n")

loss =(y) ** 2

print(f"loss:\n{loss}\n")

print("\nBACKPROPAGATION\n")

dloss_dy = 2*y
print(f"dloss_dy:\n{dloss_dy}\n")

dy_da = np.where(z > 0, 1., 0.)
print(f"dy_da:\n{dy_da}\n")

da_dz = np.ones_like(a)
print(f"da_dz:\n{da_dz}\n")

dz_dw = x
print(f"dz_dw:\n{dz_dw}\n")

dz_db = np.ones_like(b)
print(f"dz_db:\n{dz_db}\n")


dloss_dw11 = dloss_dy * x[0]
print(f"dloss_dw11:\n{dloss_dw11}\n")

dloss_dw12 = dloss_dy * x[1]
print(f"dloss_dw12:\n{dloss_dw12}\n")

dloss_dw13 = dloss_dy * x[2]
print(f"dloss_dw13:\n{dloss_dw13}\n")

dloss_dw14 = dloss_dy * x[3]
print(f"dloss_dw14:\n{dloss_dw14}\n")

dloss_db = np.ones_like(b) * dloss_dy
print(f"dloss_db:\n{dloss_db}\n")

dloss_dw41 = dloss_dw31 = dloss_dw21 = dloss_dw11
dloss_dw42 = dloss_dw32 = dloss_dw22 = dloss_dw12
dloss_dw43 = dloss_dw33 = dloss_dw23 = dloss_dw13
dloss_dw44 = dloss_dw34 = dloss_dw24 = dloss_dw14

dloss_dw = np.array([
    [dloss_dw11, dloss_dw12, dloss_dw13, dloss_dw14],
    [dloss_dw21, dloss_dw22, dloss_dw23, dloss_dw24],
    [dloss_dw31, dloss_dw32, dloss_dw33, dloss_dw34],
])

print(f"w:\n{w}\n")
print(f"dloss_dw:\n{dloss_dw}\n")

w -= 0.001 * dloss_dw

print(f"new_w:\n{w}\n")

print(f"b:\n{b}\n")
print(f"dloss_db:\n{dloss_db}\n")

b -= 0.001 * dloss_db
print(f"new_b:\n{b}\n")

