//============================================================================
// Name        : Implementacao.cpp
// Author      : Luiz Paulo Souza
// Version     :
// Copyright   : Your copyright notice
// Description : Implementação matriz blocos
//============================================================================

#include <iostream>
#include <algorithm>
#include <math.h>
#include <stdio.h>
#include <fstream>
#include <time.h>
using namespace std;

#define linhas 4
#define colunas 20
#define periodos 4
#define instantes 4
#define capacidade 64
#define porcentagem 80


static double **valores; //[linhas][colunas]
static bool ***solucao; //[linhas][colunas][periodos];
static bool **explorados; //[linhas][colunas];
static double *demanda; //[linhas];
static double **minerios; //[linhas][colunas];

static double totalDemanda;
static double porcentagensDemanda[periodos] = {17, 23, 27, 33};

static int colunaMelhorValor;
static int linhaMelhorValor;
static int colunaValorSaida;
static int linhaValorSaida;

static int quantidadeExplorados = 0;


static bool verificaExplorado(int linha, int coluna); //verifica se esta posiçao já foi explorada em algum periodo

static double calcularLucroRelativo(bool **solucaoAuxiliar, int linha, int coluna, int *quantidadeUtilizados);
static bool** copiaSolucao();
static void imprimirValores();
static void imprimirSolucao();
static void imprimirExplorados();
static void imprimirDemanda();
static void imprimirQuantidadeMinerios();
static void imprimirSolucaoAuxiliar(bool **solucaoAuxiliar);
static void iniciarSistema();
static void calcularDemandas();
static void procuraMelhorValorParaTrocar();
static bool verificaSeALinhaDeBaixoPermiteExplorar(int linha, int coluna);
static bool verificaSeALinhaDeCimaPermiteExplorar(int linha, int coluna);
static bool verificaSeOBlocoPodeSerExplorado(int linha, int coluna);
static bool verificaSePermiteTrocar(int linha, int coluna);
static void procuraValorParaTrocar(int linha, int coluna);


static void proximoEscolhido();


static bool verificaExplorado(int linha, int coluna){
    for(int i = 0 ; i < periodos ; i++){
        if(solucao[linha][coluna][i]){
            return true;
        }
    }
    return false;
}
static void gerarSolucao() {
    for (int k = periodos - 1; k >= 0; k--) {
        float demandaMinima = demanda[k];
        cout << "demanda minima " << demanda[k] << endl;
        float acumuladoPeriodo = 0;
        for (int i = linhas - 1; i >= 0; i--) {
            if (acumuladoPeriodo >= demandaMinima && k != 0) {
                break;
            }
            for (int j = 0; j < colunas; j++) {
                if (!verificaExplorado(i, j)) {

                    solucao[i][j][k] = true;
                    acumuladoPeriodo += minerios[i][j];
                    if (acumuladoPeriodo >= demandaMinima && k != 0) {
                        break;
                    }
                }
            }
        }
    }
}

int main() {
    iniciarSistema();



    ifstream fin1("matrizvalores.txt");
    if (!fin1.is_open()) {
        cout << "Erro ao abrir arquivo";
        return 0;
    }

    for (int i = 0; i < linhas; i++) { //le os valores
        for (int j = 0; j < colunas; j++) {
            fin1 >> valores[i][j];
        }
    }
    ifstream fin2("minerio2.txt");
    if (!fin2.is_open()) {
        cout << "Erro ao abrir arquivo";
        return 0;
    }

    for (int i = 0; i < linhas; i++) { //le os minerios
        for (int j = 0; j < colunas; j++) {
            fin2 >> minerios[i][j];
        }
    }


    imprimirValores();
    imprimirQuantidadeMinerios();
    calcularDemandas();
    imprimirDemanda();

    while (quantidadeExplorados < capacidade) {
        //cout << "execucao1" << endl;
        proximoEscolhido();
    }

    imprimirExplorados();
    float soma = 0;
    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            if (explorados[i][j] == true) {
                cout << valores[i][j] << "\t";
                soma += minerios[i][j];
            } else {
                cout << "0\t";
            }
        }
        cout << endl;
    }
    gerarSolucao();
    imprimirSolucao();
    cout << "total demanda : " << totalDemanda << endl;
    cout << "total pego    : " << soma << endl;


    procuraMelhorValorParaTrocar();
}

static void procuraMelhorValorParaTrocar() {
    int aux = 0;
    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            if (explorados[i][j] ==  0) {
                if  (valores[i][j] > 0) {
                    if ( verificaSeOBlocoPodeSerExplorado(i,j)) {
                        linhaMelhorValor = i;
                        colunaMelhorValor = j;
                        procuraValorParaTrocar(i,j);
                        cout << "Linha: " << i << endl;
                        cout << "Coluna: " << j << endl;
                        cout << "Valor" << aux << ": " << valores[i][j] << endl;
                        aux += 1;
                    }
                }
            }
        }
    }
}

static void procuraValorParaTrocar(int linha, int coluna) {
    bool temMelhorValor = false;
    int auxLinha, auxColuna;
    double auxValor = valores[linha][coluna];
    for (int i = linhas-1; i >= 0; i--) {
        for (int j = 0; j < colunas; j++) {
            if (verificaSePermiteTrocar(i,j)) {
                if (explorados[i][j] == 1 ) {
//                    cout<<"LINHA: "<<i<<" COLUNA: "<<j<<endl;
//                    cout<<"VALOR ANTIGO: "<<auxValor<<", VALOR NOVO: "<<valores[i][j]<<endl;
                    if (valores[i][j] < auxValor){
                        temMelhorValor = true;
                        auxLinha = i;
                        auxColuna = j;
                        cout<<"VALOR NOVO: "<<auxValor<<", VALOR ANTIGO: "<<valores[i][j]<<endl;
                        auxValor = valores[i][j];
                    }
                }
            }
        }
    }

    if (temMelhorValor) {
        explorados[linha][coluna] = 1;
        explorados[auxLinha][auxColuna] = 0;
        cout<<"MELHOROU"<<endl;
        imprimirExplorados();
        float soma = 0;
        for (int i = 0; i < linhas; i++) {
            for (int j = 0; j < colunas; j++) {
                if (explorados[i][j] == true) {
                    cout << valores[i][j] << "\t";
                    soma += minerios[i][j];
                } else {
                    cout << "0\t";
                }
            }
            cout << endl;
        }
//        gerarSolucao();
//        imprimirSolucao();
        cout << "total demanda : " << totalDemanda << endl;
        cout << "total pego    : " << soma << endl;
    }
}

static bool verificaSePermiteTrocar(int linha, int coluna) {
    if (linha == linhas-1) {
            return true;
    } else if (explorados[linha+1][coluna] == 0) {
        if (coluna == 0) {
            if (explorados[linha+1][coluna+1] == 0) { return true; }
        } else if (coluna == 19) {
            if (explorados[linha+1][coluna-1] == 0) { return true; }
        } else {
            if (explorados[linha+1][coluna-1] == 0 && explorados[linha+1][coluna-1] == 0) { return true; }
        }
    }
    return false;
}

static bool verificaSeOBlocoPodeSerExplorado(int linha, int coluna) {
    if (linha == 0) {
        return verificaSeALinhaDeBaixoPermiteExplorar(linha,coluna);
    } else if (linha == linhas-1) {
        return verificaSeALinhaDeCimaPermiteExplorar(linha,coluna);
    } else {
        return (verificaSeALinhaDeBaixoPermiteExplorar(linha,coluna) && verificaSeALinhaDeCimaPermiteExplorar(linha,coluna));
    }

    return false;
}

static bool verificaSeALinhaDeBaixoPermiteExplorar(int linha, int coluna) {
    if (explorados[linha+1][coluna] == 1) {
        if (coluna == 0) {
            if (explorados[linha+1][coluna+1] == 1) { return true; }
        } else if (coluna == 19) {
            if (explorados[linha+1][coluna-1] == 1) { return true; }
        } else {
            if (explorados[linha+1][coluna-1] == 1 && explorados[linha+1][coluna-1] == 1) { return true; }
        }
    }
    return false;

}

static bool verificaSeALinhaDeCimaPermiteExplorar(int linha, int coluna) {
    if (explorados[linha-1][coluna] == 1) {
        if (coluna == 0) {
            if (explorados[linha-1][coluna+1] == 1) { return true; }
        } else if (coluna == 19) {
            if (explorados[linha-1][coluna-1] == 1) { return true; }
        } else {
            if (explorados[linha-1][coluna-1] == 1 && explorados[linha-1][coluna-1] == 1) { return true; }
        }
    }
    return false;
}


static bool** copiaSolucao() {
    bool **auxiliar;
    auxiliar = new bool*[linhas];
    for (int i = 0; i < linhas; i++) {
        auxiliar[i] = new bool[colunas];
    }
    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            auxiliar[i][j] = explorados[i][j];
        }
    }

    return auxiliar;
}

static double calcularLucroRelativo(bool **solucaoAuxiliar, int linha, int coluna, int *quantidadeUtilizados) {
    if (linha < 0 || coluna < 0 || coluna >= colunas) {
        return 0;
    }
    if (solucaoAuxiliar[linha][coluna]) {
        return 0;
    }
    (*quantidadeUtilizados)++;
    solucaoAuxiliar[linha][coluna] = true;
    //cout << valores[linha][coluna] << " + ";
    return valores[linha][coluna] +
            calcularLucroRelativo(solucaoAuxiliar, linha - 1, coluna, quantidadeUtilizados) +
            calcularLucroRelativo(solucaoAuxiliar, linha - 1, coluna - 1, quantidadeUtilizados) +
            calcularLucroRelativo(solucaoAuxiliar, linha - 1, coluna + 1, quantidadeUtilizados);
}

static void proximoEscolhido() {

    int linhaAleatoria, colunaAleatoria;
    linhaAleatoria = (rand())%(linhas);
    colunaAleatoria = (rand())%(colunas);

    if (indiceI >= 0 && indiceJ >= 0) {
//        cout << "melhor encontrado no indice ("<<indiceI<< "," << indiceJ<<")" << endl;
        calcularLucroRelativo(explorados, linhaAleatoria, colunaAleatoria, &quantidadeExplorados);
    } else {
        cout << "NAO FOI POSSIVEL ESCOLHER NINGUEM A SER EXPLORADO" << endl;
    }
}

static void calcularDemandas() {
    double soma = 0;
    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            soma += minerios[i][j];
        }
    }
    totalDemanda = soma * porcentagem / 100;

    for (int i = 0; i < periodos; i++) {
        demanda[i] = totalDemanda * porcentagensDemanda[i] / 100;
    }
}

static void imprimirSolucao() {

    for (int k = 0; k < periodos; k++) {
        cout << (k + 1) << " PERIODO " << endl;
        for (int i = 0; i < linhas; i++) {
            for (int j = 0; j < colunas; j++) {
                if (solucao[i][j][k]) {
                    cout << "1\t";
                } else {
                    cout << "0\t";
                }
            }
            cout << endl;
        }
        cout << endl << endl;
    }

}

static void imprimirExplorados() {
    cout << "\n============================== IMPRIMINDO EXPLORADOS ==============================" << endl;
    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            if (explorados[i][j]) {
                cout << "1\t";
            } else {
                cout << "0\t";
            }
        }
        cout << endl;
    }
    cout << endl;
}

static void imprimirDemanda() {
    cout << "\n============================== IMPRIMINDO DEMANDAS ==============================" << endl;
    for (int i = 0; i < periodos; i++) {
        cout << "PERIODO " << i << ": " << demanda[i] << endl;
    }
    cout << endl;
}

static void imprimirValores() {
    cout << "\n============================== IMPRIMINDO VALORES ==============================" << endl;
    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            cout << valores[i][j] << "\t";
        }
        cout << endl;
    }
    cout << endl;
}

static void imprimirQuantidadeMinerios() {
    cout << "\n============================== IMPRIMINDO MINERIOS ==============================" << endl;
    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            cout << minerios[i][j] << "\t";
        }
        cout << endl;
    }
    cout << endl;
}

static void imprimirSolucaoAuxiliar(bool **solucaoAuxiliar) {
    cout << "\n============================== IMPRIMINDO MINERADOS (AUXILIAR) ==============================" << endl;
    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            if (solucaoAuxiliar[i][j] == true) {
                cout << "1\t";
            } else {
                cout << "0\t";
            }
        }
        cout << endl;
    }
    cout << endl;
}

static void iniciarSistema() {
    **minerios; //[linhas][colunas];

    valores = new double*[linhas];
    for (int i = 0; i < linhas; i++) {
        valores[i] = new double[colunas];
    }

    solucao = new bool**[linhas];
    for (int i = 0; i < linhas; i++) {
        solucao[i] = new bool*[colunas];
        for (int j = 0; j < colunas; j++) {
            solucao[i][j] = new bool[periodos];
            for (int k = 0; k < periodos; k++) {
                solucao[i][j][k] = false;
            }
        }
    }

    explorados = new bool*[linhas];
    for (int i = 0; i < linhas; i++) {
        explorados[i] = new bool[colunas];
        for (int j = 0; j < colunas; j++) {
            explorados[i][j] = false;
        }
    }


    demanda = new double[periodos];

    minerios = new double*[linhas];
    for (int i = 0; i < linhas; i++) {
        minerios[i] = new double[colunas];
    }
}
