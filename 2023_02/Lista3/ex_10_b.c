/*
b) A fun��o abaixo promete encontrar o valor de um elemento m�ximo de v[0..n-1]. A fun��o
cumpre a promessa?
int maxi (int n, int v[]) {
 int m = v[0];
 for (int j = 1; j < n; ++j)
 if (v[j-1] < v[j])
	 	 m = v[j];
 return m;
}
*/

//Resposta: A l�gica est� correta, por�m n�o vai funcionar pois faltou {} na declara��o da repeti��o for
