#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <conio.h>

#define USERNAME_SIZE 50
#define MAX_PASSWORD_SIZE 20
#define FILENAME_USERS "usuarios.txt"
#define FILENAME_STATS "estatisticas.txt"

typedef struct {
    char username[USERNAME_SIZE];
    char password[MAX_PASSWORD_SIZE];
} User;

typedef struct {
    char username[USERNAME_SIZE];
    int gamesPlayed;
    int wins;
    int draws;
    int losses;
} Stats;

User* users = NULL;
int userCount = 0;
int userCapacity = 2;

Stats* stats = NULL;
int statsCount = 0;
int statsCapacity = 2;

char tabuleiro[3][3];
bool partidaEmAndamento = false;
char jogadorAtual = 'X';
char ultimoJogador = ' ';
int jogador1Index = -1;
int jogador2Index = -1;

void inicializarUsuarios();
void carregarUsuarios();
void salvarUsuarios();
void carregarEstatisticas();
void salvarEstatisticas();
int encontrarIndiceEstatistica(const char* username);
void atualizarEstatisticas(const char* username, char resultado);
void ocultarSenha(char* senha);
void inicializarTabuleiro();
void exibirTabuleiro();
bool fazerJogada(int linha, int coluna, char jogador);
bool verificarVitoria(char jogador);
bool verificarEmpate();
int criarLogin();
int logarUsuario();
void jogarComLogin();
void jogarSemLogin();
void recuperarPartida();
void exibirRanking();

int main() {
    inicializarUsuarios();
    carregarUsuarios();
    carregarEstatisticas();

    int opcao;
    do {
        printf("\n===== MENU PRINCIPAL =====\n");
        printf("1. Criar login\n");
        printf("2. Jogar com login\n");
        printf("3. Jogar sem login\n");
        printf("4. Recuperar partida não concluída\n");
        printf("5. Exibir ranking\n");
        printf("0. Sair\n");
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);
        getchar();

        switch(opcao) {
            case 1:
                criarLogin();
                break;
            case 2:
                jogarComLogin();
                break;
            case 3:
                jogarSemLogin();
                break;
            case 4:
                recuperarPartida();
                break;
            case 5:
                exibirRanking();
                break;
            case 0:
                printf("Saindo...\n");
                salvarUsuarios();
                salvarEstatisticas();
                break;
            default:
                printf("Opção inválida.\n");
        }
    } while(opcao != 0);

    free(users);
    free(stats);
    return 0;
}

void inicializarUsuarios() {
    userCapacity = statsCapacity = 10;
    users = malloc(userCapacity * sizeof(User));
    stats = malloc(statsCapacity * sizeof(Stats));
}

void carregarUsuarios() {
    FILE* file = fopen(FILENAME_USERS, "r");
    if (!file) return;

    while (!feof(file)) {
        User u;
        if (fscanf(file, "%49[^,],%19s\n", u.username, u.password) == 2) {
            users[userCount++] = u;
        }
    }
    fclose(file);
}

void salvarUsuarios() {
    FILE* file = fopen(FILENAME_USERS, "w");
    for (int i = 0; i < userCount; i++) {
        fprintf(file, "%s,%s\n", users[i].username, users[i].password);
    }
    fclose(file);
}

void carregarEstatisticas() {
    FILE* file = fopen(FILENAME_STATS, "r");
    if (!file) return;

    while (!feof(file)) {
        Stats s;
        if (fscanf(file, "%49[^,],%d,%d,%d,%d\n", s.username, &s.gamesPlayed, &s.wins, &s.draws, &s.losses) == 5) {
            stats[statsCount++] = s;
        }
    }
    fclose(file);
}

void salvarEstatisticas() {
    FILE* file = fopen(FILENAME_STATS, "w");
    for (int i = 0; i < statsCount; i++) {
        fprintf(file, "%s,%d,%d,%d,%d\n", stats[i].username, stats[i].gamesPlayed, stats[i].wins, stats[i].draws, stats[i].losses);
    }
    fclose(file);
}

int encontrarIndiceEstatistica(const char* username) {
    for (int i = 0; i < statsCount; i++) {
        if (strcmp(stats[i].username, username) == 0)
            return i;
    }
    strcpy(stats[statsCount].username, username);
    stats[statsCount].gamesPlayed = stats[statsCount].wins = stats[statsCount].draws = stats[statsCount].losses = 0;
    return statsCount++;
}

void atualizarEstatisticas(const char* username, char resultado) {
    int i = encontrarIndiceEstatistica(username);
    stats[i].gamesPlayed++;
    if (resultado == 'W') stats[i].wins++;
    else if (resultado == 'E') stats[i].draws++;
    else if (resultado == 'L') stats[i].losses++;
}

void ocultarSenha(char* senha) {
    int i = 0;
    char ch;
    while ((ch = getch()) != '\r' && i < MAX_PASSWORD_SIZE - 1) {
        if (ch == 8 && i > 0) {
            printf("\b \b");
            i--;
        } else if (ch != 8) {
            senha[i++] = ch;
            printf("*");
        }
    }
    senha[i] = '\0';
    printf("\n");
}

void inicializarTabuleiro() {
    partidaEmAndamento = true;
    jogadorAtual = 'X';
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            tabuleiro[i][j] = ' ';
}

void exibirTabuleiro() {
    printf("\n");
    for (int i = 0; i < 3; i++) {
        printf(" %c | %c | %c \n", tabuleiro[i][0], tabuleiro[i][1], tabuleiro[i][2]);
        if (i < 2) printf("---|---|---\n");
    }
}

bool fazerJogada(int linha, int coluna, char jogador) {
    if (linha >= 0 && linha < 3 && coluna >= 0 && coluna < 3 && tabuleiro[linha][coluna] == ' ') {
        tabuleiro[linha][coluna] = jogador;
        return true;
    }
    return false;
}

bool verificarVitoria(char jogador) {
    for (int i = 0; i < 3; i++)
        if ((tabuleiro[i][0] == jogador && tabuleiro[i][1] == jogador && tabuleiro[i][2] == jogador) ||
            (tabuleiro[0][i] == jogador && tabuleiro[1][i] == jogador && tabuleiro[2][i] == jogador))
            return true;
    if ((tabuleiro[0][0] == jogador && tabuleiro[1][1] == jogador && tabuleiro[2][2] == jogador) ||
        (tabuleiro[0][2] == jogador && tabuleiro[1][1] == jogador && tabuleiro[2][0] == jogador))
        return true;
    return false;
}

bool verificarEmpate() {
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (tabuleiro[i][j] == ' ') return false;
    return true;
}

int criarLogin() {
    char username[USERNAME_SIZE], password[MAX_PASSWORD_SIZE];
    printf("Digite o nome de usuário: ");
    fgets(username, USERNAME_SIZE, stdin);
    username[strcspn(username, "\n")] = '\0';

    for (int i = 0; i < userCount; i++)
        if (strcmp(users[i].username, username) == 0) {
            printf("Usuário já existe.\n");
            return -1;
        }

    printf("Digite a senha: ");
    ocultarSenha(password);

    strcpy(users[userCount].username, username);
    strcpy(users[userCount].password, password);
    userCount++;
    printf("Usuário criado com sucesso!\n");
    return userCount - 1;
}

int logarUsuario() {
    char username[USERNAME_SIZE], password[MAX_PASSWORD_SIZE];
    printf("Digite o nome de usuário: ");
    fgets(username, USERNAME_SIZE, stdin);
    username[strcspn(username, "\n")] = '\0';

    printf("Digite a senha: ");
    ocultarSenha(password);

    for (int i = 0; i < userCount; i++)
        if (strcmp(users[i].username, username) == 0 &&
            strcmp(users[i].password, password) == 0)
            return i;

    printf("Usuário ou senha incorretos.\n");
    return -1;
}

void jogarComLogin() {
    printf("\n=== Jogador X ===\n");
    jogador1Index = logarUsuario();
    if (jogador1Index < 0) return;

    printf("\n=== Jogador O ===\n");
    jogador2Index = logarUsuario();
    if (jogador2Index < 0) return;

    inicializarTabuleiro();
    while (partidaEmAndamento) {
        exibirTabuleiro();
        printf("Jogador %c - linha e coluna (0 a 2): ", jogadorAtual);
        int l, c;
        scanf("%d %d", &l, &c);
        if (!fazerJogada(l, c, jogadorAtual)) {
            printf("Jogada inválida!\n");
            continue;
        }
        ultimoJogador = jogadorAtual;
        if (verificarVitoria(jogadorAtual)) {
            exibirTabuleiro();
            printf("Jogador %c venceu!\n", jogadorAtual);
            atualizarEstatisticas(users[(jogadorAtual == 'X' ? jogador1Index : jogador2Index)].username, 'W');
            atualizarEstatisticas(users[(jogadorAtual == 'X' ? jogador2Index : jogador1Index)].username, 'L');
            partidaEmAndamento = false;
        } else if (verificarEmpate()) {
            exibirTabuleiro();
            printf("Empate!\n");
            atualizarEstatisticas(users[jogador1Index].username, 'E');
            atualizarEstatisticas(users[jogador2Index].username, 'E');
            partidaEmAndamento = false;
        } else {
            jogadorAtual = (jogadorAtual == 'X') ? 'O' : 'X';
        }
    }
}

void jogarSemLogin() {
    jogador1Index = jogador2Index = -1;
    inicializarTabuleiro();
    while (partidaEmAndamento) {
        exibirTabuleiro();
        printf("Jogador %c - linha e coluna (0 a 2): ", jogadorAtual);
        int l, c;
        scanf("%d %d", &l, &c);
        if (!fazerJogada(l, c, jogadorAtual)) {
            printf("Jogada inválida!\n");
            continue;
        }
        ultimoJogador = jogadorAtual;
        if (verificarVitoria(jogadorAtual)) {
            exibirTabuleiro();
            printf("Jogador %c venceu!\n", jogadorAtual);
            atualizarEstatisticas(jogadorAtual == 'X' ? "JogadorX" : "JogadorO", 'W');
            atualizarEstatisticas(jogadorAtual == 'X' ? "JogadorO" : "JogadorX", 'L');
            partidaEmAndamento = false;
        } else if (verificarEmpate()) {
            exibirTabuleiro();
            printf("Empate!\n");
            atualizarEstatisticas("JogadorX", 'E');
            atualizarEstatisticas("JogadorO", 'E');
            partidaEmAndamento = false;
        } else {
            jogadorAtual = (jogadorAtual == 'X') ? 'O' : 'X';
        }
    }
}

void recuperarPartida() {
    if (!partidaEmAndamento) {
        printf("Nenhuma partida pendente encontrada.\n");
        return;
    }

    printf("Recuperando partida...\n");
    exibirTabuleiro();
    while (partidaEmAndamento) {
        printf("Jogador %c - linha e coluna (0 a 2): ", jogadorAtual);
        int l, c;
        scanf("%d %d", &l, &c);
        if (!fazerJogada(l, c, jogadorAtual)) {
            printf("Jogada inválida!\n");
            continue;
        }
        ultimoJogador = jogadorAtual;
        if (verificarVitoria(jogadorAtual)) {
            exibirTabuleiro();
            printf("Jogador %c venceu!\n", jogadorAtual);
            if (jogador1Index != -1 && jogador2Index != -1) {
                atualizarEstatisticas(users[(jogadorAtual == 'X' ? jogador1Index : jogador2Index)].username, 'W');
                atualizarEstatisticas(users[(jogadorAtual == 'X' ? jogador2Index : jogador1Index)].username, 'L');
            } else {
                atualizarEstatisticas(jogadorAtual == 'X' ? "JogadorX" : "JogadorO", 'W');
                atualizarEstatisticas(jogadorAtual == 'X' ? "JogadorO" : "JogadorX", 'L');
            }
            partidaEmAndamento = false;
        } else if (verificarEmpate()) {
            exibirTabuleiro();
            printf("Empate!\n");
            if (jogador1Index != -1 && jogador2Index != -1) {
                atualizarEstatisticas(users[jogador1Index].username, 'E');
                atualizarEstatisticas(users[jogador2Index].username, 'E');
            } else {
                atualizarEstatisticas("JogadorX", 'E');
                atualizarEstatisticas("JogadorO", 'E');
            }
            partidaEmAndamento = false;
        } else {
            jogadorAtual = (jogadorAtual == 'X') ? 'O' : 'X';
        }
    }
}

void exibirRanking() {
    printf("\n=== RANKING ===\n");
    for (int i = 0; i < statsCount; i++) {
        printf("%s - Jogos: %d | Vitórias: %d | Empates: %d | Derrotas: %d\n",
               stats[i].username, stats[i].gamesPlayed, stats[i].wins,
               stats[i].draws, stats[i].losses);
    }
}