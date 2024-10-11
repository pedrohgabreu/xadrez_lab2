#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINHAS 8
#define COLUNAS 8

typedef struct {
    char **tabuleiro;
    char capturadasA[16];
    char capturadasB[16];
    char **jogadas;
    int capturadasA_count;
    int capturadasB_count;
    int jogada_count;
} Jogo;

void inicializar_tabuleiro(char **tabuleiro) {
    char tabuleiro_inicial[LINHAS][COLUNAS] = {
        {'t', 'c', 'b', 'd', 'r', 'b', 'c', 't'},
        {'@', '@', '@', '@', '@', '@', '@', '@'},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {'*', '*', '*', '*', '*', '*', '*', '*'},
        {'T', 'C', 'B', 'D', 'R', 'B', 'C', 'T'}
    };

    for (int i = 0; i < LINHAS; i++) {
        for (int j = 0; j < COLUNAS; j++) {
            tabuleiro[i][j] = tabuleiro_inicial[i][j];
        }
    }
}

void imprimir_tabuleiro(char **tabuleiro) {
    printf("  A B C D E F G H\n");
    for (int i = 0; i < LINHAS; i++) {
        printf("%d ", LINHAS - i);
        for (int j = 0; j < COLUNAS ; j++) {
            printf("%c ", tabuleiro[i][j]);
        }
        printf("%d\n", LINHAS - i);
    }
    printf("  A B C D E F G H\n\n");
}

void converter_coordenadas(char coluna, char linha, int *x, int *y) {
    *x = LINHAS - (linha - '0');
    *y = coluna - 'A';
}

int movimento_torre(char **tabuleiro, int x_inicial, int y_inicial, int x_final, int y_final) {
    if (x_inicial != x_final && y_inicial != y_final) return 0;

    int dir_x = (x_final - x_inicial) ? (x_final - x_inicial) / abs(x_final - x_inicial) : 0;
    int dir_y = (y_final - y_inicial) ? (y_final - y_inicial) / abs(y_final - y_inicial) : 0;

    int x = x_inicial + dir_x;
    int y = y_inicial + dir_y;

    while (x != x_final || y != y_final) {
        if (tabuleiro[x][y] != ' ') return 0;
        x += dir_x;
        y += dir_y;
    }
    return 1;
}

int movimento_bispo(char **tabuleiro, int x_inicial, int y_inicial, int x_final, int y_final) {
    if (abs(x_final - x_inicial) != abs(y_final - y_inicial)) return 0;

    int dir_x = (x_final - x_inicial) / abs(x_final - x_inicial);
    int dir_y = (y_final - y_inicial) / abs(y_final - y_inicial);

    int x = x_inicial + dir_x;
    int y = y_inicial + dir_y;

    while (x != x_final || y != y_final) {
        if (tabuleiro[x][y] != ' ') return 0;
        x += dir_x;
        y += dir_y;
    }
    return 1;
}

int movimento_cavalo(char **tabuleiro, int x_inicial, int y_inicial, int x_final, int y_final) {
    int dx = abs(x_final - x_inicial);
    int dy = abs(y_final - y_inicial);

    return (dx == 2 && dy == 1) || (dx == 1 && dy == 2);
}

int movimento_rainha(char **tabuleiro, int x_inicial, int y_inicial, int x_final, int y_final) {
    return movimento_torre(tabuleiro, x_inicial, y_inicial, x_final, y_final) ||
           movimento_bispo(tabuleiro, x_inicial, y_inicial, x_final, y_final);
}

int movimento_rei(char **tabuleiro, int x_inicial, int y_inicial, int x_final, int y_final) {
    return abs(x_final - x_inicial) <= 1 && abs(y_final - y_inicial) <= 1;
}

int movimento_peao(char **tabuleiro, int x_inicial, int y_inicial, int x_final, int y_final, char peca) {
    int direcao = (peca == '*') ? -1 : 1;

    if (y_inicial == y_final && tabuleiro[x_final][y_final] == ' ') {
        if (x_final == x_inicial + direcao) return 1;
        if ((peca == '*' && x_inicial == 6 && x_final == 4) ||
            (peca == '@' && x_inicial == 1 && x_final == 3)) {
            return 1;
        }
    } else if (abs(y_inicial - y_final) == 1 && x_final == x_inicial + direcao &&
               tabuleiro[x_final][y_final] != ' ') {
        return 1;
    }
    return 0;
}

int movimento_valido(char **tabuleiro, int x_inicial, int y_inicial, int x_final, int y_final) {
    char peca = tabuleiro[x_inicial][y_inicial];
    char destino = tabuleiro[x_final][y_final];

    if (peca == ' ' || (peca >= 'A' && destino >= 'A' && destino <= 'Z') || (peca >= 'a' && destino >= 'a' && destino <= 'z')) {
        return 0;
    }

    switch (peca) {
        case 'T': case 't': return movimento_torre(tabuleiro, x_inicial, y_inicial, x_final, y_final);
        case 'B': case 'b': return movimento_bispo(tabuleiro, x_inicial, y_inicial, x_final, y_final);
        case 'C': case 'c': return movimento_cavalo(tabuleiro, x_inicial, y_inicial, x_final, y_final);
        case 'D': case 'd': return movimento_rainha(tabuleiro, x_inicial, y_inicial, x_final, y_final);
        case 'R': case 'r': return movimento_rei(tabuleiro, x_inicial, y_inicial, x_final, y_final);
        case '*': case '@': return movimento_peao(tabuleiro, x_inicial, y_inicial, x_final, y_final, peca);
        default: return 0;
    }
}

int verificar_xeque(char **tabuleiro, char jogador) {
    int x_rei, y_rei;
    char rei = (jogador == 'A') ? 'R' : 'r';

    for (int i = 0; i < LINHAS; i++) {
        for (int j = 0; j < COLUNAS; j++) {
            if (tabuleiro[i][j] == rei) {
                x_rei = i;
                y_rei = j;
                break;
            }
        }
    }

    for (int i = 0; i < LINHAS; i++) {
        for (int j = 0; j < COLUNAS; j++) {
            if (tabuleiro[i][j] != ' ' && movimento_valido(tabuleiro, i, j, x_rei, y_rei)) {
                return 1;
            }
        }
    }

    return 0;
}

int verificar_xeque_mate(char **tabuleiro, char jogador) {
    int x_rei, y_rei;
    char rei = (jogador == 'A') ? 'R' : 'r';


    for (int i = 0; i < LINHAS; i++) {
        for (int j = 0; j < COLUNAS; j++) {
            if (tabuleiro[i][j] == rei) {
                x_rei = i;
                y_rei = j;
                break;
            }
        }
    }


    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            int novo_x = x_rei + dx;
            int novo_y = y_rei + dy;
            if (novo_x >= 0 && novo_x < LINHAS && novo_y >= 0 && novo_y < COLUNAS) {
                if (movimento_valido(tabuleiro, x_rei, y_rei, novo_x, novo_y)) {

                    char temp = tabuleiro[novo_x][novo_y];
                    tabuleiro[novo_x][novo_y] = tabuleiro[x_rei][y_rei];
                    tabuleiro[x_rei][y_rei] = ' ';

                    if (!verificar_xeque(tabuleiro, jogador)) {

                        tabuleiro[x_rei][y_rei] = tabuleiro[novo_x][novo_y];
                        tabuleiro[novo_x][novo_y] = temp;
                        return 0;
                    }


                    tabuleiro[x_rei][y_rei] = tabuleiro[novo_x][novo_y];
                    tabuleiro[novo_x][novo_y] = temp;
                }
            }
        }
    }

    return 1;
}

void realizar_jogada(char **tabuleiro, char coluna_inicial, char linha_inicial, char coluna_final, char linha_final, char jogador) {
    int x_inicial, y_inicial, x_final, y_final;
    converter_coordenadas(coluna_inicial, linha_inicial, &x_inicial, &y_inicial);
    converter_coordenadas(coluna_final, linha_final, &x_final, &y_final);

    if (movimento_valido(tabuleiro, x_inicial, y_inicial, x_final, y_final)) {
        char peca = tabuleiro[x_inicial][y_inicial];
        char capturada = tabuleiro[x_final][y_final];

        tabuleiro[x_final][y_final] = peca;
        tabuleiro[x_inicial][y_inicial] = ' ';

        if (verificar_xeque(tabuleiro, jogador)) {
            printf("vc nao pode se colocar em xeque.\n");
            tabuleiro[x_inicial][y_inicial] = peca;
            tabuleiro[x_final][y_final] = capturada;
        } else {
            printf("movimento realizado: %c%d -> %c%d\n", coluna_inicial, linha_inicial - '0', coluna_final, linha_final - '0');


            char adversario = (jogador == 'A') ? 'B' : 'A';
            if (verificar_xeque_mate(tabuleiro, adversario)) {
                printf("xeque-mate. jogador %c vence.\n", jogador);
                exit(0);
            }
        }
    } else {
        printf("movimento invalido.\n");
    }
}

int main() {
    char **tabuleiro = (char **)malloc(LINHAS * sizeof(char *));
    for (int i = 0; i < LINHAS; i++) {
        tabuleiro[i] = (char *)malloc(COLUNAS * sizeof(char));
    }

    inicializar_tabuleiro(tabuleiro);
    imprimir_tabuleiro(tabuleiro);

    char coluna_inicial, linha_inicial, coluna_final, linha_final;

    while (1) {
        printf("digite o movimento (ex: E2 E3): ");
        scanf(" %c%c %c%c", &coluna_inicial, &linha_inicial, &coluna_final, &linha_final);

        realizar_jogada(tabuleiro, coluna_inicial, linha_inicial, coluna_final, linha_final, 'A');
        imprimir_tabuleiro(tabuleiro);
    }

    for (int i = 0; i < LINHAS; i++) {
        free(tabuleiro[i]);
    }
    free(tabuleiro);

    return 0;
}
