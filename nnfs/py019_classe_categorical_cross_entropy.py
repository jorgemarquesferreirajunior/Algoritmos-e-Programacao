import numpy as np

class Loss_CategoricalCrossEntropy:
    def calculate(self, y_pred : np.ndarray, y_true : np.ndarray):

        y_pred_clipped = np.clip(y_pred, 1e-7, 1 - 1e-7)

        # y_pred no formato [0,1,1,...]
        if len(y_true.shape) == 1:
            # obtendo apenas os valores preditos correspondentes a referencia
            filtered_y_pred_clipped = y_pred_clipped[range(len(y_true)), y_true]

            # calculando a perda de cada valor
            loss_outputs = - np.log(filtered_y_pred_clipped)

            # calculando a media da perda 
            mean_loss_outputs = np.mean(loss_outputs)

            return mean_loss_outputs

        # y_pred no fomarto one-hot encoded [[1,0,0][0,1,0],[0,1,0],...]
        elif len(y_true.shape) == 2:
            # obtendo apenas os valores preditos correspondentes a referencia
            filtered_y_pred_clipped = np.sum(y_pred_clipped * y_true, axis=1)
            # calculando a perda de cada valor
            loss_outputs = - np.log(filtered_y_pred_clipped)

            # calculando a media da perda 
            mean_loss_outputs = np.mean(loss_outputs)

            return mean_loss_outputs
        return None