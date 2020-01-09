#include<stdio.h>
#include<string.h>
#include<SDL.h>
#include"SDL_opengl.h"
#include<time.h>
#include<SDL_image.h>
#include"string"
#include<SDL_mixer.h>
//!Essa constante define o valor da velocidade dos personagens e dos projéteis
//!Caso necessário, altere esse valor para calibrar a velocidade
#define VELOCIDADE 0.1 //recomendamos aumentar de 0.1 em 0.1 até encontrar o valor ideal

//!Essa constante define as proporções da tela
#define TAMANHO_TELA 1 //recomendamos aumentar para valores entre 1 e 2



struct usuario{

    char nome[50];
    int idPersonagem;
    int vitorias;
    int derrotas;
};

struct movimento
{

    bool esq = false;
    bool dir = false;
    bool up = false;
    bool down = false;
    bool shot = false;
    int vidas = 3;
};

FILE *arq;
FILE *conexao;
bool arquivo = false; //Indica se o arquivo esta aberto
struct usuario Jogador1;
struct usuario Jogador2;

GLuint loadTexture(const std::string&fileName)
{

 SDL_Surface *imagem = IMG_Load(fileName.c_str());// carrega imagem
 SDL_DisplayFormatAlpha(imagem); //formato alpha
 unsigned object(0); //cria textura
 glGenTextures(1, &object); // gera textura
 glBindTexture(GL_TEXTURE_2D, object); // tipo de textura 2d

 glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
 glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

 glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
 glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

 glTexImage2D(GL_TEXTURE_2D, 0 , GL_RGBA, imagem ->w,imagem ->h, 0,GL_RGBA, GL_UNSIGNED_BYTE, imagem ->pixels);

 SDL_FreeSurface(imagem);

 return object;
}

void abrir_arq(const char *modo) //Abre o arquivo no modo especificado
{
    //existem varios modos de abrir um arquivo, include o arquivo pode ser binario
    //modo a+ permite leitura e escrita
	if((arq = fopen("Arquivo_SNUNS.bin", modo)) == NULL)
    {
        printf("\nErro na abertura do arquivo");
	}
	else
	{
        printf("\nAbertura bem sucedida");
    }
}

void abrir_conexao(const char *modo) //Abre o arquivo no modo especificado
{
    //existem varios modos de abrir um arquivo, include o arquivo pode ser binario
    //modo a+ permite leitura e escrita
	if((conexao = fopen("Conexao.bin", modo)) == NULL)
    {
        printf("\nErro na abertura do arquivo");
	}
}

void salvaDados_Jogador(struct usuario alvo) //Salva no arquivo as inforamções de um NOVO Jogador
{
    abrir_arq("ab+");
    arquivo = true;
    if(fwrite(&alvo, sizeof(struct usuario), 1, arq) != 1) //Escreve, em uma linha do arquivo, a struct inteira do Jogador
    {
        printf("Erro na escrita do arquivo");
	}
	else
    {
        //Dados foram escritos corretamente
        fflush(arq);//descarrega o buffer no disco
        rewind(arq);//Volta o ponteiro do arquivo para seu inicio
	}
}

void alteraDados_Jogador(struct usuario alvo) //Atualiza as informações do Jogador
{
    //Esta é um pouco mais complicada. Vou ter que procurar no arquivo a linha cujo nome gravado é o mesmo do atual jogador
    //Feito isso, devo *salvar o local* deste registro, o "numero da linha", para alterar tudo exatamente nesta linha.
    //PS: Essa função já subentende que todas as devidas alterações no estagio e pontos já foram feitas

    struct usuario Jogador_aux;
    int encontrou = 0;
    long posicao_arq;
    abrir_arq("rb+"); //Permite alterações fora do final do arquivo!!
    arquivo = true;
    // no modo ab+ funções como fseek sao ignoradas!!!!
    do{

        fread(&Jogador_aux,sizeof(struct usuario),1,arq); //Aqui, mais do que nunca, é bom usarmos uma variavel auxiliar
        if(!(strcmp(Jogador_aux.nome, alvo.nome))) //Se o nome lido for igual ao nome procurado
        {
            encontrou = 1;
            posicao_arq = ftell(arq); //ftell retorna a *posição atual do ponteiro* neste arquivo, ou seja
            //como se fosse a linha do arquivo
            fseek (arq, posicao_arq - (sizeof(struct usuario)), SEEK_SET); //Aqui a mágica é feita!!! Posiciona o ponteiro do arquivo exatamente na linha cujo nome foi encontrado!!!
            //Na verdade essa função mexe com bytes e umas coisas estranhas, não sei EXATAMENTE como funciona, mas funciona
            //posicionado o ponteiro basta escrevermos!
            arquivo = true;
            if(fwrite(&alvo, sizeof(struct usuario), 1, arq) != 1) //Escreve, em uma linha do arquivo, a struct inteira do Jogador
            {
                printf("Erro na escrita do arquivo");
            }
            else
            {
                //Dados foram escritos corretamente
                fflush(arq);//descarrega o buffer no disco
                rewind(arq);//Volta o ponteiro do arquivo para seu inicio
            }
        }
    }while(!encontrou && !feof(arq)); //Enquanto nao encountrou o registrou ou tambem não chegou ao final do arquivo
    //feof retorna 0 se o fim do arquivo vou alcançado
    //ou seja, enquanto o fim do arquivo não for alcançado devemos lê-lo
}

int carregaDados_Jogador(char nome[50], struct usuario *alvo) //Carrega informações do Jogador, cujo nome é "nome"
{
    //retorna 1 caso encontrou um jogador com o nome especificado, caso contrário, retorna 0
    struct usuario Jogador_aux; //uma variavel teste que recebera os valores
    //Crio essa variavel pois, ao final da leitura, se usasse a variavel Jogador (original), esta conteria informações da ultima linha
    abrir_arq("ab+");
    arquivo = true;
    do{

        fread(&Jogador_aux,sizeof(struct usuario),1,arq); //Aponta o poteiro para a proxima linha e carrega todas as informações automaticamente na variavel struct Jogador_aux
        if(!(strcmp(Jogador_aux.nome, nome))) //Se o nome lido for igual ao nome procurado
        {
            //Encontrou um registro com o mesmo nome!
            *alvo = Jogador_aux; //Ja que sao variaveis do mesmo tipo, esta operação é completamente viavel
            //Carregando todas as informações do "Jogador_aux" para o jogador original
            return 1;
        }

    }while(!feof(arq)); //Enquanto nao encountrou o registrou ou tambem não chegou ao final do arquivo
    //feof retorna 0 se o fim do arquivo vou alcançado
    //ou seja, enquanto o fim do arquivo não for alcançado devemos lê-lo
    return 0;

}

void exibe_dados_DEV(struct usuario alvo) //função de teste
{
    printf("\nNome: %s", alvo.nome);
    printf("\nId_personagem: %d", alvo.idPersonagem);
    printf("\nVitorias: %d", alvo.vitorias);
    printf("\nDerrotas: %d", alvo.derrotas);
}
void salva_dados_DEV(struct usuario *alvo,char nome[50], int b,int d, int e) //função de teste
{
    strcpy(alvo->nome,nome);
    alvo->idPersonagem = b;
    alvo->vitorias = d;
    alvo->derrotas = e;
}

void configurar()
{
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Surface *icone;
    icone = IMG_Load("imagens//icone.png");
    SDL_WM_SetIcon(icone, NULL);

    //memoria
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8); //transparencia
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16); //3D - não sei se vamos usar
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); //3D - não sei se vamos usar

    //Definir nome para nossa janela
    SDL_WM_SetCaption("Naruto Simulator!", NULL);

    //Tamanho da janela - (x,y, bits/pixel, tipo_janela)
    SDL_SetVideoMode(800*TAMANHO_TELA,600*TAMANHO_TELA, 32,SDL_OPENGL);

    // Cor da janea
    glClearColor(4,5,2,1);


    //Area exibida
    glViewport(0,0,800*TAMANHO_TELA,600*TAMANHO_TELA);

    //Define sombra
    glShadeModel(GL_SMOOTH);

    //2D (COOL)
    glMatrixMode(GL_PROJECTION); //Aplica a projeção da matriz atual
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);

    //Para as imagens
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    int frequencia = 22050;
    Uint16 formato = AUDIO_S16SYS;
    int canais = 2;
    int buffer = 4096;

    Mix_OpenAudio(frequencia, formato, canais, buffer);
}

bool colisao(float X1, float Y1, float comp1, float alt1, float X2, float Y2, float comp2, float alt2)
{
    //Detectar colisão em todos os lados
    if(Y1 + alt1 < Y2) //Se colide com a parte de cima
        return false;
    else if(Y1 > Y2 + alt2) //Se colide com a parte de baixo
        return false;
    else if(X1 + comp1 < X2) //Se colide com a frente
        return false;
    else if(X1 > X2 + comp2) //Se colide com a parte de traz
        return false;

    return true;
}

void carregaDados_Jogador_Conexao() //Carrega informações dos dois jogadores ativos!
{
    //Considerando que os jogadores ja digitaram seus nomes e escolheram seus cadastros
    //Ou se eles já tiverem jogado, considerando que seus registros já foram devidamente salvos no arquivo conexão
    //Essa função le este arquivo conexao e carrega os unicos dois registros gravados neste nos struct usuario Jogador1 e Jogador2

    abrir_conexao("ab+"); //Neste programa, meu unico interesse é ler o arquivo
    arquivo = true;
    fread(&Jogador1,sizeof(struct usuario), 1, conexao);
    fread(&Jogador2,sizeof(struct usuario), 1, conexao);
    fclose(conexao);

}

void salvaDados_Jogadores_Conexao() //Salva no arquivo as inforamções de um NOVO Jogador
{
    abrir_conexao("wb+");
    fwrite(&Jogador1, sizeof(struct usuario), 1, conexao); //Escreve, em uma linha do arquivo, a struct inteira do Jogador
    fwrite(&Jogador2, sizeof(struct usuario), 1, conexao);//Escreve, em uma linha do arquivo, a struct inteira do Jogador
    fflush(arq);//descarrega o buffer no disco
    rewind(arq);//Volta o ponteiro do arquivo para seu inicio
    fclose(conexao);
}

void escreve_vidas(int vidas1, int vidas2, float cLado, unsigned int coracao)
{
    //Função que escreve os respectivos corações para os dois jogadores
    int i;
    float x, y = 570;
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, coracao); //!SETA A IMAGEM

    //inicia desenho
    glBegin(GL_QUADS);

    //Para o jogador 1 (esquerda)
    x = 20;
    for(i = 0 ; i < vidas1 ; i++)
    {
        glTexCoord2d(0,0);/* -->*/glVertex2f(x, y); //primeiro ponto
        glTexCoord2d(1,0);        glVertex2f(x + cLado, y); //segundo ponto
        glTexCoord2d(1,1);        glVertex2f(x + cLado, y + cLado);
        glTexCoord2d(0,1);        glVertex2f(x, y + cLado);
        x += cLado + 2;
    }

    //Para o jogador 2 (direita)

    x = 760;
    for(i = 0 ; i < vidas2 ; i++)
    {
        glTexCoord2d(0,0);/* -->*/glVertex2f(x, y); //primeiro ponto
        glTexCoord2d(1,0);        glVertex2f(x + cLado, y); //segundo ponto
        glTexCoord2d(1,1);        glVertex2f(x + cLado, y + cLado);
        glTexCoord2d(0,1);        glVertex2f(x, y + cLado);
        x -= cLado + 2;
    }

    glEnd();
    glDisable(GL_TEXTURE_2D);


}

time_t Tick[2];
time_t t_coracao;
void movimento(struct movimento *J1, struct movimento *J2, float *J1X, float *J1Y, float *J2X, float *J2Y,
                float **tiros1, float **tiros2, float J1Comp, float J1Alt, float J2Comp, float J2Alt, bool *coracao1, bool *coracao2,
                float *c1X, float *c1Y, float *c2X, float *c2Y, float cLado) //Processa todos os movimentos necessários, e retorna os valores das coordernadas
{
    int i;

    if(J1->esq && *J1X > 0) //O ponto inicial dele
        *J1X -= VELOCIDADE * 2;

    if(J1->dir && *J1X + J1Comp < 400) //Trava o movimento do personagem no quadrado estabelecido
        *J1X += VELOCIDADE * 2;

    if(J2->esq && *J2X > 400) //se a tecla A for pressionada
        *J2X -= VELOCIDADE * 2;

    if(J2->dir && *J2X + J2Comp < 800) //se a tecla D for pressionada
        *J2X += VELOCIDADE * 2;

    if(J1->up && *J1Y > 0)
        *J1Y -= VELOCIDADE * 2;

    if(J1->down && *J1Y + J1Alt < 550)
        *J1Y += VELOCIDADE * 2;

    if(J2->up && *J2Y > 0)
        *J2Y -= VELOCIDADE * 2;

    if(J2->down && *J2Y + J2Alt < 550)
        *J2Y += VELOCIDADE * 2;

    if(J1->shot)
    {
        J1->shot = false;
        for(i = 0; i < 100 ; i++)
        {
            if(!tiros1[i][2]) //Se a terceira coluna de uma das linhas de tiro estiver "Vaga" (for igual a zero)
            {
                tiros1[i][0] =  *J1X + J1Comp; //O tiro começa na mesma posição do personagem
                tiros1[i][1] =  *J1Y + (J1Alt)/2 -35;
                tiros1[i][2] = 1.0; //indica que ha esse tiro
                i = 100;
            }
        }
    }

    if(J2->shot)
    {
        J2->shot = false;
        for(i = 0; i < 100; i++)
        {
            if(!tiros2[i][2])
            {
                tiros2[i][0] = *J2X - J2Comp;
                tiros2[i][1] = *J2Y + (J2Alt)/2 -35;
                tiros2[i][2] = 1.0; //indica que há esse tiro
                i = 100;
            }
        }
    }
    //Verificar se devo gerar corações
    time_t tempo = time(NULL);
    if((double) (tempo - t_coracao) >= 10) //Deve enviar o comando para gerar um coração em posição aleatoria a cada 12.5s
    {
        t_coracao = tempo;
        *coracao1 = true;
        *coracao2 = true;

        *c1X = rand()%400 - cLado; //!Adicionar aqui os tamanhos corretos dos corações!
        *c1Y = rand()%550 - cLado;
        *c2X = 400 + rand()%400 - cLado;
        *c2Y = rand()%550 - cLado;
    }
}
void carrega_imagem_player(unsigned int *player1, unsigned int *player2)
{

    switch (Jogador1.idPersonagem)
    {
        case 1 : *player1 = loadTexture("imagens//naruto.png");
        //printf ("\nPersonagem: Naruto");
        break;

        case 2 : *player1 = loadTexture("imagens//sasuke.png");
        //printf ("\nPersonagem: Sasuke");
        break;

        case 3 : *player1 = loadTexture("imagens//sakura.png");
        //printf ("\nPersonagem: Sakura");
        break;

        case 4 : *player1 = loadTexture("imagens//pain.png");
        //printf ("\nPersonagem: Pain");
        break;

        case 5 : *player1 = loadTexture("imagens//gaara.png");
        //printf ("\nPersonagem: Gaara");
        break;

        case 6 : *player1 = loadTexture("imagens//jiraya.png");
        //printf ("\nPersonagem: Jiraiya");
        break;

        case 7 : *player1 = loadTexture("imagens//kakashi.png");
        //printf ("\nPersonagem: Kakashi");
        break;

        case 8 : *player1 = loadTexture("imagens//orochimaru.png");
        //printf ("\nPersonagem: Orochimaru");
        break;
    }

    switch (Jogador2.idPersonagem)
    {
        case 1 : *player2 = loadTexture("imagens//naruto.png");
        //printf ("\nPersonagem: Naruto");
        break;

        case 2 : *player2 = loadTexture("imagens//sasuke.png");
        //printf ("\nPersonagem: Sasuke");
        break;

        case 3 : *player2 = loadTexture("imagens//sakura.png");
        //printf ("\nPersonagem: Sakura");
        break;

        case 4 : *player2 = loadTexture("imagens//pain.png");
        //printf ("\nPersonagem: Pain");
        break;

        case 5 : *player2 = loadTexture("imagens//gaara.png");
        //printf ("\nPersonagem: Gaara");
        break;

        case 6 : *player2 = loadTexture("imagens//jiraya.png");
        //printf ("\nPersonagem: Jiraiya");
        break;

        case 7 : *player2 = loadTexture("imagens//kakashi.png");
        //printf ("\nPersonagem: Kakashi");
        break;

        case 8 : *player2 = loadTexture("imagens//orochimaru.png");
        //printf ("\nPersonagem: Orochimaru");
        break;
    }


}
int main(int argc, char* args[])
{
    carregaDados_Jogador_Conexao();

    float **tiros1; //A primeira coluna é a posição X, a segunda Y, a terceira indica se aquele tiro ainda existe
    float **tiros2; //A primeira coluna é a posição X, a segunda Y, a terceira indica se aquele tiro ainda existe
    int i, vencedor, musica_toca, conta_tempo;

    tiros1 = (float **) calloc(20, sizeof(float *)); //!Acho dificil ter mais de 20 tiros ativos, mas se tiver dando pau pode ser nisso
    tiros2 = (float **) calloc(20, sizeof(float *));

    for(i = 0; i < 20; i++)
    {
        tiros1[i] = (float *) calloc(3, sizeof(float));
        tiros2[i] = (float *) calloc(3, sizeof(float));
    }

    Tick[0] = Tick[1] = time(NULL);

    arquivo = false;
    configurar();

    Mix_Chunk *som;
    Mix_Chunk *dano;
    Mix_Music *musica1;
    Mix_Music *musica2;
    som = Mix_LoadWAV("sons//alarme.wav");
    dano = Mix_LoadWAV("sons//dano.wav");
    musica1 = Mix_LoadMUS("sons//batalha.mp3");
    musica2 = Mix_LoadMUS("sons//vitoria.mp3");

    Mix_VolumeMusic(MIX_MAX_VOLUME/3);
    Mix_VolumeChunk(dano, MIX_MAX_VOLUME*2);

    time_t inicio, tempo;



    //!------------ LOGICA AQUI EM BAIXO ---------------------
    bool executando = true;
    bool coracao1 = false; //Variavel que indica se um coração deve ser gerado em local aleatório
    bool coracao2 = false;
    bool comecou_jogo = false;
    bool creditos = false;
    bool instrucoes = false;
    bool acabou_jogo = false;

    //Varial que detecta clickes de teclas
    SDL_Event eventos;

    //Iniciando variaveis para elementos e personagens
    float J1X = 50.0;
    float J1Y = 300.0;
    float J1Comp = 90.0;
    float J1Alt = 110.0;

    float TiroComp  = 70.0;
    float TiroAlt = 20.0;


    float J2X = 700.0;
    float J2Y = 300.0;
    float J2Comp = J1Comp;
    float J2Alt = J1Alt;

    float c1X = 900; //Variáveis que armazenas as coordenadas X e Y dos possiveis corações gerados;
    float c1Y = 500; //Começam com esses valores para que eles começem fora da tela
    float c2X = 900;
    float c2Y = 500;
    float cLado = 25; //Coração quadrado


    //Variáveis para movimentar o personagem
    struct movimento J1;
    struct movimento J2;

    //Para as imagens
    unsigned int player1 = 0;
    unsigned int player2 = 0;
    unsigned int menu = 0;
    unsigned int fundo = 0;
    unsigned int kunai1 = 0,kunai2 = 0;
    unsigned int coracao = 0;
    unsigned int vitoria = 0;
    unsigned int creditos_img = 0;
    unsigned int instrucoes_img = 0;

    ///Função deve detectar a imagem corretamente, dado os ids dos personagens
    carrega_imagem_player(&player1, &player2);
    menu = loadTexture("imagens//menu.png");
    fundo = loadTexture("imagens//fundo2.png");
    vitoria = loadTexture("imagens//vitoria.png");
    kunai1 = loadTexture("imagens//kunai1.png");
    kunai2 = loadTexture("imagens//kunai2.png");
    coracao = loadTexture("imagens//coracao.png");
    creditos_img = loadTexture("imagens//creditos.png");
    instrucoes_img = loadTexture("imagens//instrucoes.png");



    //loop do jogo
    while(executando)
    {

        //eventos
        while(SDL_PollEvent(&eventos))
        {
            //Fecha com o X da janela
            if(eventos.type == SDL_QUIT)
                executando = false;

            //Detectar movimento

            if(eventos.type == SDL_KEYDOWN) //Se uma tecla foi pressionada!!
            { //enquanto a tecla esta pressionada, movimentar
                if(eventos.key.keysym.sym == SDLK_a) //Apertou seta esquerda
                    J1.esq = true;

                if(eventos.key.keysym.sym == SDLK_d) //Apertou seta direita
                    J1.dir = true;

                if(eventos.key.keysym.sym == SDLK_LEFT)
                    J2.esq = true;

                if(eventos.key.keysym.sym == SDLK_RIGHT)
                    J2.dir = true;

                if(eventos.key.keysym.sym == SDLK_w)
                    J1.up = true;

                if(eventos.key.keysym.sym == SDLK_s)
                    J1.down = true;

                if(eventos.key.keysym.sym == SDLK_UP)
                    J2.up = true;

                if(eventos.key.keysym.sym == SDLK_DOWN)
                    J2.down = true;

                if(eventos.key.keysym.sym == SDLK_SPACE)
                {
                    tempo = time(NULL);
                    if(comecou_jogo && (double) (tempo - Tick[0]) >= 1.5) //A diferença entre o tiro anterior e esse tem que ser maior que 1s
                    {
                        Tick[0] = tempo;
                        J1.shot = true;
                    }
                }
                if(eventos.key.keysym.sym == SDLK_SLASH)
                {
                    tempo = time(NULL);
                    if(comecou_jogo && (double) (tempo - Tick[1]) >= 1.5) //A diferença entre o tiro anterior e esse tem que ser maior que 1s
                    {
                        Tick[1] = tempo;
                        J2.shot = true;
                    }
                }
                if(eventos.key.keysym.sym == SDLK_1)
                { //Botao jogar
                    if(!creditos && !comecou_jogo && !instrucoes) //Garante que está  no menu, ou seja, nenhuma das  outras telas está ativa
                    {
                        Mix_PlayMusic(musica1, -1);
                        t_coracao = time(NULL); //Inicia o contador dos coraçãoes corretamente, após o início do jogo
                        comecou_jogo = true;
                    }
                    //Mix_HaltMusic(); para parar
                }
                if(eventos.key.keysym.sym == SDLK_2)
                { //Botao creditos
                    if(!creditos && !comecou_jogo && !instrucoes) //Garante que está  no menu, ou seja, nenhuma das  outras telas está ativa
                    {
                        creditos = true;
                        inicio = time(NULL);
                    }
                }
                if(eventos.key.keysym.sym == SDLK_3)
                { //Botao instrucoes
                    if(!creditos && !comecou_jogo && !instrucoes) //Garante que está  no menu, ou seja, nenhuma das  outras telas está ativa
                    {
                        instrucoes = true;
                        inicio = time(NULL);
                    }
                }

            }

            else if(eventos.type == SDL_KEYUP)
            { //se a tecla foi solta, para de se mover
                if(eventos.key.keysym.sym == SDLK_a) //Apertou seta esquerda
                    J1.esq = false;

                if(eventos.key.keysym.sym == SDLK_d) //Apertou seta direita
                    J1.dir = false;

                if(eventos.key.keysym.sym == SDLK_LEFT)
                    J2.esq = false;

                if(eventos.key.keysym.sym == SDLK_RIGHT)
                    J2.dir = false;

                if(eventos.key.keysym.sym == SDLK_w)
                    J1.up = false;

                if(eventos.key.keysym.sym == SDLK_s)
                    J1.down = false;

                if(eventos.key.keysym.sym == SDLK_UP)
                    J2.up = false;

                if(eventos.key.keysym.sym == SDLK_DOWN)
                    J2.down = false;

            }
        }


        //!!RENDERIZAÇÃO!!
        glClear(GL_COLOR_BUFFER_BIT); //limpa o buffer

        //Inicia Matriz
        glPushMatrix();


        //Configura a matriz
        glOrtho(0, 800, 600, 0, -1, 1);

        if(!comecou_jogo && !creditos && !instrucoes) //Ativa a tela de  menu
        {
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, menu); //!SETA A IMAGEM

                //inicia desenho
                glBegin(GL_QUADS);

                glColor4ub(255,255,255,255);
                glTexCoord2d(0,0);   glVertex2f(0, 0);//primeiro ponto
                glTexCoord2d(1,0);   glVertex2f(800, 0); // segundo ponto
                glTexCoord2d(1,1);   glVertex2f(800, 600);
                glTexCoord2d(0,1);   glVertex2f(0, 600);

                glEnd();
                glDisable(GL_TEXTURE_2D);
        }
        else if(creditos) //Ativa a tela de créditos
        {
                tempo = time(NULL);

                if((double) tempo - inicio < 15)
                {
                    glEnable(GL_TEXTURE_2D);
                    glBindTexture(GL_TEXTURE_2D, creditos_img); //!SETA A IMAGEM

                    //inicia desenho
                    glBegin(GL_QUADS);

                    glColor4ub(255,255,255,255);
                    glTexCoord2d(0,0);   glVertex2f(0, 0);//primeiro ponto
                    glTexCoord2d(1,0);   glVertex2f(800, 0); // segundo ponto
                    glTexCoord2d(1,1);   glVertex2f(800, 600);
                    glTexCoord2d(0,1);   glVertex2f(0, 600);

                    glEnd();
                    glDisable(GL_TEXTURE_2D);
                }
                else
                    creditos = false;
        }
        else if(instrucoes) //Ativa a tela de instruções
        {
                tempo = time(NULL);

                if((double) tempo - inicio < 15)
                {
                    glEnable(GL_TEXTURE_2D);
                    glBindTexture(GL_TEXTURE_2D, instrucoes_img); //!SETA A IMAGEM

                    //inicia desenho
                    glBegin(GL_QUADS);

                    glColor4ub(255,255,255,255);
                    glTexCoord2d(0,0);   glVertex2f(0, 0);//primeiro ponto
                    glTexCoord2d(1,0);   glVertex2f(800, 0); // segundo ponto
                    glTexCoord2d(1,1);   glVertex2f(800, 600);
                    glTexCoord2d(0,1);   glVertex2f(0, 600);

                    glEnd();
                    glDisable(GL_TEXTURE_2D);
                }
                else
                    instrucoes = false;
        }
        else if(comecou_jogo) //comecou_jogo, ativa o jogo
        {
            //-----------Setando o fundo ----//
            if(!acabou_jogo)
            {

                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, fundo); //!SETA A IMAGEM

                //inicia desenho
                glBegin(GL_QUADS);

                glColor4ub(255,255,255,255);
                glTexCoord2d(0,0);   glVertex2f(0, 0);//primeiro ponto
                glTexCoord2d(1,0);   glVertex2f(800, 0); // segundo ponto
                glTexCoord2d(1,1);   glVertex2f(800, 600);
                glTexCoord2d(0,1);   glVertex2f(0, 600);

                glEnd();
                glDisable(GL_TEXTURE_2D);

                //!LOGICA DO PROGRAMA -- movimento do personagem
                movimento(&J1, &J2, &J1X, &J1Y, &J2X, &J2Y, tiros1, tiros2, J1Comp, J1Alt, J2Comp, J2Alt, &coracao1, &coracao2, &c1X, &c1Y, &c2X, &c2Y, cLado); //Processa todo o movimento necessário

                //-------------Setando outra imagem ---------//
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, player1); //!SETA A IMAGEM

                //inicia desenho
                glBegin(GL_QUADS); //GL_POINTS, GL_LINES, GL_LINES_LOOP, GL_TRIANGLE, GL_POLIGON

                //Desenhando jogador1
                glColor4ub(255,255,255,255);
                glTexCoord2d(0,0); glVertex2f(J1X, J1Y); //primeiro ponto
                glTexCoord2d(1,0); glVertex2f(J1X + J1Comp, J1Y); //segundo ponto
                glTexCoord2d(1,1); glVertex2f(J1X + J1Comp, J1Y + J1Alt);
                glTexCoord2d(0,1); glVertex2f(J1X, J1Y + J1Alt);

                glEnd();
                glDisable(GL_TEXTURE_2D);

                //------------SETA OUTRA IMAGEM -----------------//


                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, player2); //!SETA A IMAGEM

                //inicia desenho
                glBegin(GL_QUADS);

                //Desenhando Jogador2
                glColor4ub(255,255,255,255);
                glTexCoord2d(0,0); glVertex2f(J2X, J2Y); //primeiro ponto
                glTexCoord2d(1,0); glVertex2f(J2X + J2Comp, J2Y); //segundo ponto
                glTexCoord2d(1,1); glVertex2f(J2X + J2Comp, J2Y + J2Alt);
                glTexCoord2d(0,1); glVertex2f(J2X, J2Y + J2Alt);
                glEnd();
                glDisable(GL_TEXTURE_2D);

                //Desenhando os corações
                if(coracao2 && J2.vidas < 5)
                {
                    glEnable(GL_TEXTURE_2D);
                    glBindTexture(GL_TEXTURE_2D, coracao); //!SETA A IMAGEM
                    //inicia desenho
                    glBegin(GL_QUADS);

                    glColor4ub(255,255,255,255);
                    glTexCoord2d(0,0);/* -->*/glVertex2f(c2X, c2Y); //primeiro ponto
                    glTexCoord2d(1,0);        glVertex2f(c2X + cLado, c2Y); //segundo ponto
                    glTexCoord2d(1,1);        glVertex2f(c2X + cLado, c2Y + cLado);
                    glTexCoord2d(0,1);        glVertex2f(c2X, c2Y + cLado);

                    glEnd();
                    glDisable(GL_TEXTURE_2D);
                }
                if(coracao1 && J1.vidas < 5)
                {
                    glEnable(GL_TEXTURE_2D);
                    glBindTexture(GL_TEXTURE_2D, coracao); //!SETA A IMAGEM
                    //inicia desenho
                    glBegin(GL_QUADS);

                    glColor4ub(255,255,255,255);
                    glTexCoord2d(0,0);   glVertex2f(c1X, c1Y); //primeiro ponto
                    glTexCoord2d(1,0);   glVertex2f(c1X + cLado, c1Y); //segundo ponto
                    glTexCoord2d(1,1);   glVertex2f(c1X + cLado, c1Y + cLado);
                    glTexCoord2d(0,1);   glVertex2f(c1X, c1Y + cLado);

                    glEnd();
                    glDisable(GL_TEXTURE_2D);
                }

                for(i = 0; i < 20; i++) //Busca nas duas matrizes de tiro por projeteis que tenham sido atirados
                { //Aqui os projetos serão desenhados, e as colisões serão detectadas

                    //Atualiza as posições dos tiros
                    if(tiros1[i][2] != 0.0) // Coordenadas do tiro são (tiro1[i][0] ; tiros1[i][1])
                    {
                        glEnable(GL_TEXTURE_2D);
                        glBindTexture(GL_TEXTURE_2D, kunai1); //!SETA A IMAGEM
                        glBegin(GL_QUADS);

                        if(tiros1[i][0] + TiroComp> 800) //Se saiu da tela
                            tiros1[i][2] = 0.0; //Mata o bloco
                        //Desenha - atualiza a posição dos tiros

                        //Ve se teve colisão
                        if(colisao(J2X, J2Y, J2Comp, J2Alt, tiros1[i][0], tiros1[i][1], TiroComp, TiroAlt)) //Ocorreu colisão
                        {
                            //Colisão com o Jogador2

                            //Ativa o som do dano
                            Mix_PlayChannel(-1,dano, 0);

                            tiros1[i][2] = 0.0;
                            J2.vidas--; //Tira uma das vidas do jogador2
                            if(!J2.vidas) //Se não tem mais vidas, ou seja morreu
                            {
                                //Acaba o jogo
                                Jogador1.vitorias++;
                                Jogador2.derrotas++;

                                acabou_jogo = true;
                                vencedor = 1;
                                musica_toca = 1;
                                inicio = time(NULL); //Inicia a contagem para a tela de vitoria

                            }
                        }
                        else
                        {   //Caso não tenha ocorrido colisão, o tiro deve continuar seu trajeto
                            glColor4ub(255,255,255,255);
                            glTexCoord2d(0,0); glVertex2f(tiros1[i][0], tiros1[i][1]); //primeiro ponto
                            glTexCoord2d(1,0); glVertex2f(tiros1[i][0] + TiroComp, tiros1[i][1]); //segundo ponto
                            glTexCoord2d(1,1); glVertex2f(tiros1[i][0] + TiroComp, tiros1[i][1] + TiroAlt);
                            glTexCoord2d(0,1); glVertex2f(tiros1[i][0], tiros1[i][1] + TiroAlt);
                            tiros1[i][0] += VELOCIDADE * 4; //Atualiza a posição do tiro
                        }
                        glEnd();
                        glDisable(GL_TEXTURE_2D);
                    }
                    if(tiros2[i][2] != 0.0)
                    {
                        glEnable(GL_TEXTURE_2D);
                        glBindTexture(GL_TEXTURE_2D, kunai2); //!SETA A IMAGEM
                        glBegin(GL_QUADS);

                        if(tiros2[i][0] < 0)
                            tiros2[i][2] = 0.0;
                        //Detectar colisão com os personagens, projetil por projetil!!!!

                        //Ve se teve colisão // Some com o tiro, computa o dano
                        if(colisao(J1X, J1Y, J1Comp, J1Alt, tiros2[i][0], tiros2[i][1], TiroComp, TiroAlt))
                        {
                            tiros2[i][2] = 0.0;
                            //Colidiu com o jogador1;
                            Mix_PlayChannel(-1, dano, 0); //Ativa  o som

                            J1.vidas--;
                            if(!J1.vidas)
                            {
                                //O jogador1 morreu, o jogo acaba
                                Jogador1.derrotas++;
                                Jogador2.vitorias++;
                                //Salva os dados devidamente
                                acabou_jogo = true;
                                vencedor = 2;
                                musica_toca = 1;
                                inicio = time(NULL);

                            }
                        }
                        else
                        {
                            glColor4ub(255,255,255, 255);
                            glTexCoord2d(0,0); glVertex2f(tiros2[i][0], tiros2[i][1]); //primeiro ponto
                            glTexCoord2d(1,0); glVertex2f(tiros2[i][0] + TiroComp, tiros2[i][1]); //segundo ponto
                            glTexCoord2d(1,1); glVertex2f(tiros2[i][0] + TiroComp, tiros2[i][1] + TiroAlt);
                            glTexCoord2d(0,1); glVertex2f(tiros2[i][0], tiros2[i][1] + TiroAlt);
                            tiros2[i][0] -= VELOCIDADE * 4;
                        }

                        glEnd();
                        glDisable(GL_TEXTURE_2D);
                    }
                }
                //Detecta se  houve colisão com o coração, ou seja, se os jogadores pegaram seus corações extras;

                if(colisao(J1X, J1Y, J1Comp, J1Alt, c1X, c1Y, cLado, cLado) && coracao1 && J1.vidas < 5) //NOVAMENTE, AQUI ARRUMAR OS COMPRIMENTOS E ALTURAS DOS CORAÇÕES
                { //O fato de o coração não estar desenhado não impede a função colisao de retornar true
                    J1.vidas++;
                    coracao1 = false;
                }
                if(colisao(J2X, J2Y, J2Comp, J2Alt, c2X, c2Y, cLado, cLado) && coracao2 && J2.vidas < 5) //NOVAMENTE, AQUI ARRUMAR OS COMPRIMENTOS E ALTURAS DOS CORAÇÕES
                { //Aqui, se eu não testar se coracao2 é falso, mesmo que o coração suma, quando o jogador "Colide" com as coordenadas dele, ele vai ganhar uma vida

                    J2.vidas++;
                    coracao2 = false;
                }

                escreve_vidas(J1.vidas, J2.vidas, cLado, coracao); //escreve na tela as vidas dos personagens
            }
            else //acabou o jogo, mostrar vitorioso
            {
                tempo = time(NULL);
                if(musica_toca == 1) //Troca a musica, e a mantém até o final
                {
                    alteraDados_Jogador(Jogador1); //Como entrara aqui so uma vez, ja salva todos os dados necessarios
                    alteraDados_Jogador(Jogador2);
                    salvaDados_Jogadores_Conexao(); //Salva no arquivo ponte as informações recentes
                    //Isso é bom pois possibilita revanche sem ter que passar pelo menu
                    Mix_HaltMusic();
                    Mix_VolumeMusic(MIX_MAX_VOLUME/2);
                    Mix_PlayMusic(musica2, -1);
                    musica_toca = 0;

                }

                if((double) (tempo - inicio) < 15) //A tela deve ficar 15s a mostra
                {

                    glEnable(GL_TEXTURE_2D);
                    glBindTexture(GL_TEXTURE_2D, vitoria); //!SETA A IMAGEM

                    //inicia desenho
                    glBegin(GL_QUADS);

                    glColor4ub(255,255,255,255);
                    glTexCoord2d(0,0);   glVertex2f(0, 0);//primeiro ponto
                    glTexCoord2d(1,0);   glVertex2f(800, 0); // segundo ponto
                    glTexCoord2d(1,1);   glVertex2f(800, 600);
                    glTexCoord2d(0,1);   glVertex2f(0, 600);

                    glEnd();
                    glDisable(GL_TEXTURE_2D);

                    //inicia desenho do personagemm
                    glEnable(GL_TEXTURE_2D);
                    if(vencedor == 1)
                        glBindTexture(GL_TEXTURE_2D, player1); //!SETA A IMAGEM
                    else
                        glBindTexture(GL_TEXTURE_2D, player2); //!SETA A IMAGEM


                    glBegin(GL_QUADS);

                    //Desenhando jogador1
                    J1Comp = 300;
                    J1Alt = 380;
                    J1X = 10;
                    J1Y = 150;
                    glColor4ub(255,255,255,255);
                    glTexCoord2d(0,0); glVertex2f(J1X, J1Y); //primeiro ponto
                    glTexCoord2d(1,0); glVertex2f(J1X + J1Comp, J1Y); //segundo ponto
                    glTexCoord2d(1,1); glVertex2f(J1X + J1Comp, J1Y + J1Alt);
                    glTexCoord2d(0,1); glVertex2f(J1X, J1Y + J1Alt);

                    glEnd();
                    glDisable(GL_TEXTURE_2D);
                }
                else
                    executando = false;

            }
        }
        //Fecha matriz
        glPopMatrix();

        //Animação -- precisa definir mas é meio inutil
        SDL_GL_SwapBuffers();

    }


    for(i = 0; i < 20; i++)
    {
        free(tiros1[i]);
        free(tiros2[i]);
    }

    free(tiros1);
    free(tiros2);
    Mix_FreeChunk(som);
    Mix_FreeChunk(dano);
    Mix_FreeMusic(musica1);
    Mix_FreeMusic(musica2);

    Mix_CloseAudio();
    glDisable(GL_BLEND);
    SDL_Quit();

    if(arquivo)
        fclose(arq);

    return 0;
}
