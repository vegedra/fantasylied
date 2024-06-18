/******************************************************************************

                            	Fantasy Lied
                             by Pedro Ivo, 2024

*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h> // Biblioteca PDCurses (curses pra windows)

// Constantes para definir o número de salas/rooms e o tamanho do nome e descrição
#define NUM_ROOMS 99
#define NAME_SIZE 15
#define DESC_SIZE 256
#define NUM_EVENTS 10

// Estrutura de uma sala/local - room
typedef struct {
    char name[NAME_SIZE];
    char description[DESC_SIZE];
    int north;
    int south;
    int east;
    int west;
	int eventIndex;  // Evento que a sala terá (-1 se não houver)
} Room;

typedef struct {
	char description[DESC_SIZE];
	int effect; // Efeito do evento, pode ser dano
} Event;

// Variavéis
Room rooms[NUM_ROOMS];          // Array das Rooms
Event events[NUM_EVENTS];		// Array dos eventos
int currentRoomIndex = 0;       // Index da room atual
char player_move[10];           // Input do movimento do jogador
int player_hp = 10;             // Pontos de vida do jogador, se chegar a 0 ele morre
char name[9];                   // Nome do jogador

// Protótipos de função
void displayPlayerStats();
void handleMovement();
void generateCharacter();
void initializeRooms();
void initializeEvents();

int main(void) {
    // Inicializa PDCurses
    initscr();          // Inicia a tela
    cbreak();           // Desabilita line buffering
    keypad(stdscr, TRUE); // Habilita caracteres especiais
    //noecho();           // Desativa a exibição de teclas pressionadas

    // Inicializa as salas e eventos
    initializeRooms();
	initializeEvents();
	
	int input;
    do {
		curs_set(0);  // Esconde o cursor
		resize_term(40, 80);  // Muda o tamanho da janela, tem que estar acompanhado de um refresh()
        clear();
        mvprintw(0, 0, "********************************************************************************");
        mvprintw(1, 34, "Fantasy Lied");
        mvprintw(2, 31, "by Pedro Ivo, 2024");
        mvprintw(20, 36, "1) Play");
        mvprintw(21, 36, "2) Exit");
        mvprintw(39, 0, "********************************************************************************\n");
		refresh();  // Atualiza a tela

        // Recebe o input do jogador
        input = getch();

        if (input == '1') {
			curs_set(1);
            generateCharacter();
			
            do {
                handleMovement();  // Movimentação do jogador
				
				if (player_hp <= 0) {
					mvprintw(20, 0, "YOU'RE DEAD. GAME OVER");
					refresh();
					napms(2000);
					break;
				}
            } while (strcasecmp(player_move, "sair") != 0);
        }
    } while (input != '2');
    mvprintw(14, 0, "\nExiting.");
    refresh();
    endwin();
    return 0;
}

// Inicia os eventos
void initializeEvents() {
	strcpy(events[0].description, "Um espinho sai do chão e te atinge! -2 de HP!");
	events[0].effect = -2;

    strcpy(events[1].description, "Você encontrou uma poção de cura e ganhou 3 pontos de vida.");
    events[1].effect = 3;
	
	// Evento padrão
	strcpy(events[2].description, "Não há nada anormal aqui.");
    events[2].effect = 0;
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
	rooms[0].eventIndex = 2;  // Sem evento

	// Sala Floresta
    strcpy(rooms[1].name, "Floresta");
    strcpy(rooms[1].description, "Uma floresta densa e misteriosa.");
    rooms[1].north = -1; // No room to the north
    rooms[1].south = 0;  // South leads back to room 0 (Cave)
    rooms[1].east = -1;  // No room to the east
    rooms[1].west = -1;  // No room to the west
	rooms[1].eventIndex = 0; // Armadilha

	// Sala deserto
    strcpy(rooms[2].name, "Deserto");
    strcpy(rooms[2].description, "Quente e árido...");
    rooms[2].north = 0; // North leads back to room 0 (Cave)
    rooms[2].south = -1; // No room to the south
    rooms[2].east = -1;  // No room to the east
    rooms[2].west = -1;  // No room to the west
	rooms[2].eventIndex = 2; // Sem evento

	// Sala castelo
    strcpy(rooms[3].name, "Castelo");
    strcpy(rooms[3].description, "Em ruínas...");
    rooms[3].north = -1; // No room to the north
    rooms[3].south = -1; // No room to the south
    rooms[3].east = -1;  // No room to the east
    rooms[3].west = 0;   // West leads back to room 0 (Cave)
	rooms[3].eventIndex = 2; // Sem evento
}

// Exibe a localização e a descrição atuais do jogador
void displayPlayerStats() {
    mvprintw(0, 0, "%s está em %s  |  HP: %d\n", name, rooms[currentRoomIndex].name, player_hp);
    mvprintw(1, 0, "%s\n", rooms[currentRoomIndex].description);
	
	// debug
	//mvprintw(10, 0, "evento da room atual %d\n", rooms[currentRoomIndex].eventIndex); 
    
    // Verifica se há um evento associado à sala atual
    if (rooms[currentRoomIndex].eventIndex >= 0) {
        mvprintw(2, 0, "%s\n", events[rooms[currentRoomIndex].eventIndex].description);
    } 
    refresh();
}

// Cuida do movimento do jogador com base no input do mesmo
void handleMovement() {
    int newRoomIndex = -1;
    char directions[][10] = {"norte", "sul", "leste", "oeste", "sair"};
    int validDirection = 0;

    while (1) {
        clear();
        displayPlayerStats();
        mvprintw(4, 0, "Escolha uma direção cardinal ou 'sair' para voltar ao menu inicial.");
        mvprintw(5, 0, "> ");
        refresh();

        scanw("%9s", player_move);

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
            break;
        }

        // Avalia a entrada do jogador
        if (validDirection) {
            if (newRoomIndex >= 0) {
                currentRoomIndex = newRoomIndex;

                // Lida com evento na nova sala
                if (rooms[currentRoomIndex].eventIndex >= 0) {
                    player_hp += events[rooms[currentRoomIndex].eventIndex].effect;  // Aplica o efeito do evento
                } break; // Movimento válido, sai do loop
            } else {
                mvprintw(9, 0, "Não há saída nessa direção. Tente novamente.");
                refresh();
                getch(); // Pausa até que o jogador pressione uma tecla
            }
        } else {
            mvprintw(9, 0, "Comando inválido. Tente novamente.");
            refresh();
            getch(); // Pausa até que o jogador pressione uma tecla
        }
    }
}


// Geração de personagem
void generateCharacter() {
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
	mvprintw(13, 0, "Pressione Enter para continuar");
    refresh();
    getch();
}
