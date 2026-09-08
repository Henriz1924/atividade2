
#include <stdio.h>
#include <stdlib.h>
#include "raylib.h"

#define LARGURA_JANELA   800
#define ALTURA_JANELA    600
#define QUANTIDADE_MOEDAS 15
#define TEMPO_REAPARECER  3.0f
typedef enum {
    MOEDA_BRONZE,
    MOEDA_PRATA,
    MOEDA_OURO,
    MOEDA_DIAMANTE
} TipoMoeda;

typedef struct {
    Vector2   pos;
    float     raio;
    TipoMoeda tipo;
    int       valor;
    bool      coletada;
    float     tempoColeta;
} Moeda;

Color corDaMoeda(TipoMoeda tipo) {
    switch (tipo) {
        case MOEDA_BRONZE:   return (Color){160, 90, 40, 255};
        case MOEDA_PRATA:    return (Color){190, 190, 190, 255};
        case MOEDA_OURO:     return GOLD;
        case MOEDA_DIAMANTE: return (Color){80, 220, 255, 255}; 
        default:              return WHITE;
    }
}

int valorDaMoeda(TipoMoeda tipo) {
    switch (tipo) {
        case MOEDA_BRONZE:   return 5;
        case MOEDA_PRATA:    return 10;
        case MOEDA_OURO:     return 25;
        case MOEDA_DIAMANTE: return 50;
        default:              return 0;
    }
}

TipoMoeda sortearTipoMoeda(void) {
    int chanceRara = GetRandomValue(0, 9);
    if (chanceRara == 0) {
        return MOEDA_DIAMANTE;
    }
    return (TipoMoeda) GetRandomValue(MOEDA_BRONZE, MOEDA_OURO);
}

void reposicionarMoeda(Moeda *m) {
    m->pos  = (Vector2){ (float) GetRandomValue(30, LARGURA_JANELA - 30),
                          (float) GetRandomValue(30, ALTURA_JANELA - 30) };
    m->tipo  = sortearTipoMoeda();
    m->valor = valorDaMoeda(m->tipo);
}

Moeda *criarMoedas(int quantidade) {
    Moeda *moedas = (Moeda *) malloc(quantidade * sizeof(Moeda));
    for (int i = 0; i < quantidade; i++) {
        Moeda *m = (moedas + i);
        m->raio        = 10.0f;
        m->coletada    = false;
        m->tempoColeta = 0.0f;
        reposicionarMoeda(m);
    }
    return moedas;
}

bool tentarColetar(Moeda *m, Vector2 posJogador, float raioJogador) {
    if (m->coletada) return false;

    float dx = m->pos.x - posJogador.x;
    float dy = m->pos.y - posJogador.y;
    float distancia  = (dx * dx + dy * dy);
    float somaRaios  = (m->raio + raioJogador) * (m->raio + raioJogador);

    if (distancia <= somaRaios) {
        m->coletada    = true;
        m->tempoColeta = GetTime();
        return true;
    }
    return false;
}

/*
 * Exercicio 1: percorre o vetor a cada quadro e faz as moedas coletadas
 * reaparecerem, em nova posicao aleatoria, apos TEMPO_REAPARECER segundos.
 */
void atualizarMoedas(Moeda *moedas, int quantidade) {
    for (int i = 0; i < quantidade; i++) {
        Moeda *m = (moedas + i);
        if (m->coletada && (GetTime() - m->tempoColeta) >= TEMPO_REAPARECER) {
            reposicionarMoeda(m);
            m->coletada = false;
        }
    }
}

void desenharMoeda(Moeda *m) {
    if (m->coletada) return;
    DrawCircleV(m->pos, m->raio, corDaMoeda(m->tipo));
}

int main(void) {
    InitWindow(LARGURA_JANELA, ALTURA_JANELA, "Atividade 2 - Enum + Vetor Dinamico de Structs");
    SetTargetFPS(60);

    Moeda *moedas = criarMoedas(QUANTIDADE_MOEDAS);

    Vector2 posJogador  = { LARGURA_JANELA / 2.0f, ALTURA_JANELA / 2.0f };
    float   raioJogador = 15.0f;
    float   velocidade  = 250.0f;
    int     pontuacao   = 0;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) posJogador.x += velocidade * dt;
        if (IsKeyDown(KEY_LEFT)  || IsKeyDown(KEY_A)) posJogador.x -= velocidade * dt;
        if (IsKeyDown(KEY_DOWN)  || IsKeyDown(KEY_S)) posJogador.y += velocidade * dt;
        if (IsKeyDown(KEY_UP)    || IsKeyDown(KEY_W)) posJogador.y -= velocidade * dt;

        if (posJogador.x < raioJogador) posJogador.x = raioJogador;
        if (posJogador.x > LARGURA_JANELA - raioJogador) posJogador.x = LARGURA_JANELA - raioJogador;
        if (posJogador.y < raioJogador) posJogador.y = raioJogador;
        if (posJogador.y > ALTURA_JANELA - raioJogador) posJogador.y = ALTURA_JANELA - raioJogador;

        for (int i = 0; i < QUANTIDADE_MOEDAS; i++) {
            Moeda *m = (moedas + i);
            if (tentarColetar(m, posJogador, raioJogador)) {
                pontuacao += m->valor;
            }
        }

        /* Exercicio 1: verifica reaparecimento das moedas coletadas */
        atualizarMoedas(moedas, QUANTIDADE_MOEDAS);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        for (int i = 0; i < QUANTIDADE_MOEDAS; i++) {
            desenharMoeda(moedas + i);
        }

        DrawCircleV(posJogador, raioJogador, BLUE);

        DrawText(TextFormat("Pontuacao: %d", pontuacao), 10, 10, 20, DARKGRAY);
        DrawText("Bronze=5  Prata=10  Ouro=25  Diamante=50", 10, 35, 16, DARKGRAY);
        DrawText("Mova-se com WASD ou setas", 10, ALTURA_JANELA - 25, 16, GRAY);

        EndDrawing();
    }

    free(moedas);

    CloseWindow();
    return 0;
}
