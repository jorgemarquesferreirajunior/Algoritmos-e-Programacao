#include "Equipamento.h"
#include<iostream>
#include<cstring>

Equipamento::Equipamento()
{
    //ctor
}

Equipamento::~Equipamento()
{
    //dtor
}

// metodos setters
void Equipamento::setNome(const char *_nome)
{
    strcpy(nome,_nome);
}

void Equipamento::setFabricante(const char *_fabricante)
{
    strcpy(fabricante,_fabricante);
}

void Equipamento::setPreco(float _preco)
{
    preco = _preco;
}

// metodos getters
char* Equipamento::getNome (void)
{
    return nome;
}
char* Equipamento::getFabricante (void)
{
    return fabricante;
}
float Equipamento::getPreco(void)
{
    return preco;
}


