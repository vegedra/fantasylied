#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>  // Habilitar caracteres especiais
#include <curses.h> // Biblioteca PDCurses (curses pra windows)

// Constantes para definir o número de salas/rooms e o tamanho do nome e descrição
#define NUM_ROOMS 4
#define NAME_SIZE 15
#define DESC_SIZE 256

// Estrutura de uma sala/local - room
typedef struct {
    char name[NAME_SIZE];
    char description[DESC_SIZE];
    int north;
    int south;
    int east;
    int west;
} Room;

// Variavéis
Room rooms[NUM_ROOMS];          // Array das Rooms
int currentRoomIndex = 0;       // Index da room atual
char player_move[10];           // Input do movimento do jogador
int player_hp = 10;             // Pontos de vida do jogador, se chegar a 0 ele morre

// Protótipos de função
void displayPlayerStats();
void handleMovement();
void generateCharacter();
void initializeRooms();

int main(void) {
    // Seta o locale para o portugues e que mostre os caracteres especiais
    setlocale(LC_ALL, "pt_BR.UTF-8");

 

    // Inicializa PDCurses
    initscr();          // Inicia a tela
    cbreak();           // Disable line buffering
    keypad(stdscr, TRUE); // Enable special keys
    curs_set(0);        // Hide the cursor
    //noecho();           // Desativa a exibição de teclas pressionadas

    // Inicializa as salas
    initializeRooms();
	
	int input;
    do {
		resize_term(40, 80);  // Muda o tamanho da janela, tem que estar acompanhado de um refresh()
        clear();
        mvprintw(0, 0, "********************************************************************************");
        mvprintw(1, 34, "Fantasy Lied\u00E7");
        mvprintw(2, 31, "by Pedro Ivo, 2024");
        mvprintw(20, 36, "1) Jogar");
        mvprintw(21, 36, "2) Sair");
        mvprintw(39, 0, "********************************************************************************");
		refresh();  // Atualiza a tela

        // Recebe o input do jogador
        input = getch();

        if (input == '1') {
			curs_set(1);
            generateCharacter();
            do {
                clear();
                displayPlayerStats();  // Mostra informações ao jogador
				
                handleMovement();  // Movimentação do jogador
            } while (strcasecmp(player_move, "sair") != 0);
        }
    } while (input != '2');
    mvprintw(14, 0, "\nSaindo do jogo.");
    refresh();
    endwin();
    return 0;
}

// Inicia as rooms com seus nomes, descrições e conexões predefinidas
void initializeRooms() {
    //strcpy(destino, fonte);  syntaxe

	// Sala Caverna
    strcpy(rooms[0].name, "Caverna");
    strcpy(rooms[0].description, "Uma caverna escura.");
    rooms[0].north = 1; // North leads to room 1 (Forest)
    rooms[0].south = 2; // South leads to room 2 (Desert)
    rooms[0].east = 3;  // East leads to room 3 (Castle)
    rooms[0].west = -1; // No room to the west (-1 means no exit)

	// Sala Floresta
    strcpy(rooms[1].name, "Floresta");
    strcpy(rooms[1].description, "Uma floresta densa e misteriosa.");
    rooms[1].north = -1; // No room to the north
    rooms[1].south = 0;  // South leads back to room 0 (Cave)
    rooms[1].east = -1;  // No room to the east
    rooms[1].west = -1;  // No room to the west

	// Sala deserto
    strcpy(rooms[2].name, "Deserto");
    strcpy(rooms[2].description, "Quente e árido...");
    rooms[2].north = 0; // North leads back to room 0 (Cave)
    rooms[2].south = -1; // No room to the south
    rooms[2].east = -1;  // No room to the east
    rooms[2].west = -1;  // No room to the west

	// Sala castelo
    strcpy(rooms[3].name, "Castelo");
    strcpy(rooms[3].description, "Em ruínas...");
    rooms[3].north = -1; // No room to the north
    rooms[3].south = -1; // No room to the south
    rooms[3].east = -1;  // No room to the east
    rooms[3].west = 0;   // West leads back to room 0 (Cave)
}

// Exibe a localização e a descrição atuais do jogador
void displayPlayerStats() {
    mvprintw(0, 0, "Você está em: %s  |  HP: %d\n", rooms[currentRoomIndex].name, player_hp);
    mvprintw(1, 0, "%s\n", rooms[currentRoomIndex].description);
}

// Cuida do movimento do jogador com base no input do mesmo
void handleMovement() {
    int newRoomIndex = -1;
    char directions[][10] = {"norte", "sul", "leste", "oeste", "sair"};
    int validDirection = 0; // Flag para verificar se a direção é válida

    while (1) {
        clear(); // Limpa a tela para nova entrada
        displayPlayerStats(); // Mostra a posição atual e HP do jogador
        mvprintw(4, 0, "Escolha uma direção cardinal, digite 'sair' para voltar ao menu inicial.");
        mvprintw(5, 0, "> ");
        refresh();

        scanw("%9s", player_move); // Limita o input a 9 caracteres

        // Reseta a direção e a flag de direção válida
        newRoomIndex = -1;
        validDirection = 0;

        // Verifica a direção inserida
        if (strcasecmp(player_move, directions[0]) == 0) { // "norte"
            newRoomIndex = rooms[currentRoomIndex].north;
            validDirection = 1;
        } else if (strcasecmp(player_move, directions[1]) == 0) { // "sul"
            newRoomIndex = rooms[currentRoomIndex].south;
            validDirection = 1;
        } else if (strcasecmp(player_move, directions[2]) == 0) { // "leste"
            newRoomIndex = rooms[currentRoomIndex].east;
            validDirection = 1;
        } else if (strcasecmp(player_move, directions[3]) == 0) { // "oeste"
            newRoomIndex = rooms[currentRoomIndex].west;
            validDirection = 1;
        } else if (strcasecmp(player_move, directions[4]) == 0) { // "sair"
            break; // Sai do loop se o jogador digitar 'sair'
        }

        // Avalia a entrada do jogador
        if (validDirection) {
            if (newRoomIndex >= 0) {
                currentRoomIndex = newRoomIndex;
                break; // Movimento válido, sai do loop
            } else {
                mvprintw(8, 0, "Não há saída nessa direção. Tente novamente.");
                refresh();
                napms(1000); // Pausa para que o jogador veja a mensagem
            }
        } else {
            mvprintw(8, 0, "Comando inválido. Tente novamente.");
            refresh();
            napms(1000); // Pausa para que o jogador veja a mensagem
        }
    }
}

// Geração de personagem
void generateCharacter() {
    char name[9];

    clear();
    mvprintw(0, 0, "Diga-me, qual o seu nome?");
	mvprintw(1, 0, "> ");
    refresh();
    scanw("%9s", name); 

    mvprintw(3, 0, "Há milênios existiu uma terra rica e próspera, alguns dizem ter sido o\n"
					"berço da humanidade e da magia, porém, com o ataque e regime cruel de 1000\n"
					"anos do Herói Traidor Matin'ornom, o arquipélago de Solerania desapareceu.\n");
	
	mvprintw(7, 0, "Dizem que quem encontrar a terra perdida poderá se tornar Rei e possuir o que\n"
					"magos e estudiosos matariam para ter: todo o conhecimento de Mundia.");
	
	mvprintw(10, 0, "Saindo de seu humilde vilarejo, %s embarca em uma jornada até o Reino Perdido.", name);
	refresh();
	//napms(2000);  // sleep do curses.h
	mvprintw(12, 0, "Pressione Enter para continuar");
    refresh();
    getch();
}
