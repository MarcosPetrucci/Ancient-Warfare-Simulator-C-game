#include<stdio.h>
#include<stdlib.h>
#include<string.h>

// PARA ARQUIVOS BINARIOS:

struct usuario{

    char nome[50];
    int idPersonagem;
    int vitorias;
    int derrotas;
};

FILE *arq;
FILE *conexao;
bool arquivo = false; //Indica se o arquivo esta aberto
bool acabou_jogo = false; //Indica se o jogo acabou ou não
struct usuario Jogador1;
struct usuario Jogador2;

void abrir_arq(const char *modo) //Abre o arquivo no modo especificado
{
    //existem varios modos de abrir um arquivo, include o arquivo pode ser binario
    //modo a+ permite leitura e escrita
	if((arq = fopen("Arquivo_SNUNS.bin", modo)) == NULL)
    {
        printf("\nErro na abertura do arquivo");
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

void salvaDados_Jogador1_Conexao(struct usuario alvo) //Salva no arquivo as inforamções de um NOVO Jogador
{
    abrir_conexao("wb+");
    arquivo = true;
    if(fwrite(&alvo, sizeof(struct usuario), 1, conexao) != 1) //Escreve, em uma linha do arquivo, a struct inteira do Jogador
    {
        printf("Erro na escrita do arquivo cone 1");
	}
	else
    {
        //Dados foram escritos corretamente
        fflush(arq);//descarrega o buffer no disco
        rewind(arq);//Volta o ponteiro do arquivo para seu inicio
	}
}
void salvaDados_Jogador2_Conexao(struct usuario alvo) //Salva no arquivo as inforamções de um NOVO Jogador
{
    abrir_conexao("ab+");
    arquivo = true;
    if(fwrite(&alvo, sizeof(struct usuario), 1, conexao) != 1) //Escreve, em uma linha do arquivo, a struct inteira do Jogador
    {
        printf("Erro na escrita do arquivo cone 2");

	}
	else
    {
        //Dados foram escritos corretamente
        fflush(arq);//descarrega o buffer no disco
        rewind(arq);//Volta o ponteiro do arquivo para seu inicio
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

void exibe_personagem_pelo_id(struct usuario alvo){

    int ninja = alvo.idPersonagem;

    switch (ninja)
    {
        case 1 :
        printf ("\nPersonagem: Naruto");
        break;

        case 2 :
        printf ("\nPersonagem: Sasuke");
        break;

        case 3 :
        printf ("\nPersonagem: Sakura");
        break;

        case 4 :
        printf ("\nPersonagem: Pain");
        break;

        case 5 :
        printf ("\nPersonagem: Gaara");
        break;

        case 6 :
        printf ("\nPersonagem: Jiraiya");
        break;

        case 7 :
        printf ("\nPersonagem: Kakashi");
        break;
        
        case 8 :
        printf ("\nPersonagem: Orochimaru");
        break;
    }
}

void lista_id_personagens(){

    printf("\n1-Naruto");
    printf("\n2-Sasuke");
    printf("\n3-Sakura");
    printf("\n4-Pain");
    printf("\n5-Gaara");
    printf("\n6-Jiraiya");
    printf("\n7-Kakashi");
    printf("\n8-Orochimaru\n\n");
    printf("Escolha seu personagem (digite seu numero): ");

}

void exibe_dados(struct usuario alvo)
{
    exibe_personagem_pelo_id(alvo);
    printf("\nVitorias: %d", alvo.vitorias);
    printf("\nDerrotas: %d\n", alvo.derrotas);
}
void salva_dados_DEV(struct usuario *alvo,char nome[50], int b,int d, int e) //função de teste
{
    strcpy(alvo->nome,nome);
    alvo->idPersonagem = b;
    alvo->vitorias = d;
    alvo->derrotas = e;
}

void opcao_jogar(void){

    char nomeDigitado1[50];
    char nomeDigitado2[50];
	printf("\t\t\t---------- SUPER NARUTO ULTIMATE NINJA FIGTHER SIMULATOR 19 ----------");
    printf("\n############### Jogador 1 ################\n\n");

    printf("Digite seu nome ninja: ");
    fgets(nomeDigitado1, 50, stdin);

    if(carregaDados_Jogador(nomeDigitado1, &Jogador1) == 1){

        exibe_dados(Jogador1);
        salvaDados_Jogador1_Conexao(Jogador1);
        printf("\nUm ninja experiente!!\n");
        fclose(conexao);

    }

    else{

        strcpy(Jogador1.nome, nomeDigitado1);

        printf("Um novo nija!\n");

        lista_id_personagens();
        scanf("%d", &Jogador1.idPersonagem);
        printf("\nBoa escolha!!\n");

        Jogador1.vitorias = 0;
        Jogador1.derrotas = 0;

        salvaDados_Jogador(Jogador1);
        salvaDados_Jogador1_Conexao(Jogador1);
        fclose(conexao);
    }

    fflush(stdin);

    printf("\n");

    printf("############### Jogador 2 ################\n\n");

    printf("Digite seu nome ninja: ");
    fgets(nomeDigitado2, 50, stdin);

    if(carregaDados_Jogador(nomeDigitado2, &Jogador2) == 1){

        exibe_dados(Jogador2);
        printf("\nUm ninja experiente!!\n");
        salvaDados_Jogador2_Conexao(Jogador2);
    }

    else{

        strcpy(Jogador2.nome, nomeDigitado2);

        printf("Um novo nija!\n");

        lista_id_personagens();
        scanf("%d", &Jogador2.idPersonagem);
        printf("\nBoa escolha!!\n\n");

        Jogador2.vitorias = 0;
        Jogador2.derrotas = 0;

        salvaDados_Jogador(Jogador2);
        salvaDados_Jogador2_Conexao(Jogador2);
    }
	printf("Bom jogo %c voc%cs!! Basta executar o Super Naruto U. N. F. e jogar com seus personagens!!", 133,136 );

    fclose(arq);
    fclose(conexao);
}


int carregaDados_Jogador_Conexao(char nome[50], struct usuario *alvo) //Carrega informações do Jogador, cujo nome é "nome"
{
    //retorna 1 caso encontrou um jogador com o nome especificado, caso contrário, retorna 0

    struct usuario Jogador_aux; //uma variavel teste que recebera os valores

    //Crio essa variavel pois, ao final da leitura, se usasse a variavel Jogador (original), esta conteria informações da ultima linha

    abrir_conexao("ab+");
    arquivo = true;
    do{

        fread(&Jogador_aux,sizeof(struct usuario), 1,conexao); //Aponta o poteiro para a proxima linha e carrega todas as informações automaticamente na variavel struct Jogador_aux
        if(!(strcmp(Jogador_aux.nome, nome))) //Se o nome lido for igual ao nome procurado
        {
            //Encontrou um registro com o mesmo nome!
            *alvo = Jogador_aux; //Ja que sao variaveis do mesmo tipo, esta operação é completamente viavel
            //Carregando todas as informações do "Jogador_aux" para o jogador original
            return 1;
        }

    }while(!feof(conexao)); //Enquanto nao encountrou o registrou ou tambem não chegou ao final do arquivo
    //feof retorna 0 se o fim do arquivo vou alcançado
    //ou seja, enquanto o fim do arquivo não for alcançado devemos lê-lo
    return 0;

}
void testa ()
{
	char nomeDigitado1[50];

    printf("Digite seu nome ninja: ");
    fgets(nomeDigitado1, 50, stdin);

    if(carregaDados_Jogador_Conexao(nomeDigitado1, &Jogador1) == 1){

        exibe_dados(Jogador1);
    }
    else
    	printf("fedeu");
    
}
int main()
{
	char lixo[3];
    opcao_jogar();
    printf("\nAperte enter para sair");
    gets(lixo);
    //testa();
    return 0;
}
