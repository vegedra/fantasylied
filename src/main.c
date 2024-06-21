/******************************************************************************

                        EXIT THE DUNGEON IN 60 SECONDS
                            by Pedro Ivo, 2024

*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h> // Biblioteca PDCurses (curses pra windows)
#include <time.h>

// Constantes para definir o número de salas/rooms e o tamanho do nome e descrição
#define NUM_ROOMS 99
#define NAME_SIZE 35
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
Event events[NUM_EVENTS];	    // Array dos eventos
int currentRoomIndex = 0;       // Index da room atual
char player_move[10];           // Input do movimento do jogador
char name[9];                   // Nome do jogador
int bonus;			            // Bonus que é calculado no SCORE
int steps = 0;                  // Contador de passos do jogador

// Protótipos de função
void displayPlayerStats(int remaining_time);
void handleMovement(int remaining_time);
void generateCharacter();
void initializeRooms();
void initializeEvents();
void calculateScore(int remaining_time);

int main(void) {
    // Inicializa PDCurses
    initscr();               // Inicia a tela
    cbreak();                // Desabilita line buffering
    keypad(stdscr, TRUE);    // Habilita caracteres especiais
    //noecho();              // Desativa a exibição de teclas pressionadas

    // Inicializa as salas e eventos
    initializeRooms();
	initializeEvents();
	
	int input;
    do {
		curs_set(0);  // Esconde o cursor
		resize_term(40, 80);  // Muda o tamanho da janela, tem que estar acompanhado de um refresh()
        clear();
        mvprintw(0, 0, "********************************************************************************");
        mvprintw(1, 25, "EXIT THE DUNGEON IN 60 SECONDS");
        mvprintw(2, 31, "by Pedro Ivo, 2024");
        mvprintw(20, 36, "1) Play");
        mvprintw(21, 36, "2) Exit");
		mvprintw(38, 75, "v1.0");
        mvprintw(39, 0, "********************************************************************************\n");
		refresh();  // Atualiza a tela

        // Recebe o input do jogador
        input = getch();

        if (input == '1') {
			curs_set(1);

            generateCharacter();
			
            time_t start_time = time(NULL);  // Get the current time
            int remaining_time = 60;  // Timer set to 60 seconds

			//timeout(1000);
			
            do {
				// Calcula o tempo que passou
				int elapsed_time = (int)difftime(time(NULL), start_time);
                remaining_time = 60 - elapsed_time;

                handleMovement(remaining_time);  // Pass remaining time to handleMovement
                
                if (remaining_time <= 0 && currentRoomIndex != 9) {
                    mvprintw(20, 0, "THE MONSTER GOT YOU! GAME OVER");
                    refresh();
                    napms(2000);
                    break;
				}
				
                // Atualiza a tela
                clear();
                displayPlayerStats(remaining_time);
                refresh();
				
				// Se o jogador chegou ao final
                if (currentRoomIndex == 9) {
                    calculateScore(remaining_time);
                    refresh();
                    getch();
					break;
                }
                
            } while (strcasecmp(player_move, "exit") != 0 && remaining_time > 0);
			// Reset game state if player chooses to play again
            currentRoomIndex = 0;
            bonus = 0;
            steps = 0;
            strcpy(player_move, "");
            strcpy(name, "");
        }
    } while (input != '2');
    mvprintw(14, 0, "\nExiting.");
    refresh();
    endwin();
    return 0;
}

// Inicia os eventos
void initializeEvents() {
	strcpy(events[0].description, "You found the secret!");
	events[0].effect = +10;
}

// Inicia as rooms com seus nomes, descrições e conexões predefinidas
void initializeRooms() {
    //strcpy(destino, fonte);  syntaxe

	// Level 1
	// A0
    strcpy(rooms[0].name, "is at the start");
    strcpy(rooms[0].description, "I can hear the monster...");
    rooms[0].north = -1; // Não
    rooms[0].south = -1; // Não
    rooms[0].east = 1;  // East leads to room 1 
    rooms[0].west = -1; // Não
	rooms[0].eventIndex = -1;  // Sem evento

	// B1
    strcpy(rooms[1].name, "is in a corridor.");
    strcpy(rooms[1].description, "I'm hungry.");
    rooms[1].north = 2; 
    rooms[1].south = -1;  
    rooms[1].east = -1;  
    rooms[1].west = 0;  
	rooms[1].eventIndex = -1;

	// C2
    strcpy(rooms[2].name, "is in a corridor (again?)");
    strcpy(rooms[2].description, "Oh God.");
    rooms[2].north = 3; 
    rooms[2].south = 1; 
    rooms[2].east = -1;  
    rooms[2].west = 4;  
	rooms[2].eventIndex = 2; 

	// D3
    strcpy(rooms[3].name, "is in a dead-end.");
    strcpy(rooms[3].description, "Opsie...");
    rooms[3].north = -1;
    rooms[3].south = 2;
    rooms[3].east = -1;  
    rooms[3].west = -1;   
	rooms[3].eventIndex = -1;
	
	// E4
    strcpy(rooms[4].name, "is in a world of possibilities.");
    strcpy(rooms[4].description, "Which one...");
    rooms[4].north = 5;
    rooms[4].south = -1;
    rooms[4].east = 2;  
    rooms[4].west = 7;   
	rooms[4].eventIndex = -1;
	
	// F5
    strcpy(rooms[5].name, "is so close!");
    strcpy(rooms[5].description, "...");
    rooms[5].north = 9;
    rooms[5].south = 4;
    rooms[5].east = -1;  
    rooms[5].west = 6;   
	rooms[5].eventIndex = -1;
	
	// G6
    strcpy(rooms[6].name, "is in -- what's that?");
    strcpy(rooms[6].description, "Still a dead-end...");
    rooms[6].north = -1;
    rooms[6].south = -1;
    rooms[6].east = 5;  
    rooms[6].west = -1;   
	rooms[6].eventIndex = 0;
	
	// H7
    strcpy(rooms[7].name, "is in -- where am I?");
    strcpy(rooms[7].description, "...?");
    rooms[7].north = -1;
    rooms[7].south = 8;
    rooms[7].east = 4;  
    rooms[7].west = -1;   
	rooms[7].eventIndex = -1;
	
	// I8
    strcpy(rooms[8].name, "is in a dead-end.");
    strcpy(rooms[8].description, "Opsie...");
    rooms[8].north = 7;
    rooms[8].south = -1;
    rooms[8].east = -1;  
    rooms[8].west = -1;   
	rooms[8].eventIndex = -1;
	
	// FINAL
    strcpy(rooms[9].name, "got out of the dungeon!");
    strcpy(rooms[9].description, "Congratulations!");
    rooms[9].north = -1;
    rooms[9].south = -1;
    rooms[9].east = -1;  
    rooms[9].west = -1;   
	rooms[9].eventIndex = -1;
}

// Exibe a localização e a descrição atuais do jogador
void displayPlayerStats(int remaining_time) {
    mvprintw(0, 0, "%s %s | TIME LEFT: %d seconds.\n", name, rooms[currentRoomIndex].name, remaining_time);
    mvprintw(1, 0, "%s\n", rooms[currentRoomIndex].description);
	
	// debug
	//mvprintw(10, 0, "player move: %s\n", player_move); 
    
    // Verifica se há um evento associado à sala atual
    if (rooms[currentRoomIndex].eventIndex >= 0) {
        mvprintw(2, 0, "%s\n", events[rooms[currentRoomIndex].eventIndex].description);
    } 
    refresh();
}

// Cuida do movimento do jogador com base no input do mesmo
void handleMovement(int remaining_time) {
    int newRoomIndex = -1;
    int validDirection = 0;

    while (1) {
        clear();
        displayPlayerStats(remaining_time);
        mvprintw(4, 0, "Type a direction or 'exit' to return to the main menu.");
        mvprintw(5, 0, "> ");
        refresh();

        scanw("%9s", player_move);

        newRoomIndex = -1;
        validDirection = 0;

        // Check the input direction and set the newRoomIndex accordingly
        if (strcasecmp(player_move, "north") == 0) {
            newRoomIndex = rooms[currentRoomIndex].north;
            validDirection = 1;
        } else if (strcasecmp(player_move, "south") == 0) {
            newRoomIndex = rooms[currentRoomIndex].south;
            validDirection = 1;
        } else if (strcasecmp(player_move, "east") == 0) {
            newRoomIndex = rooms[currentRoomIndex].east;
            validDirection = 1;
        } else if (strcasecmp(player_move, "west") == 0) {
            newRoomIndex = rooms[currentRoomIndex].west;
            validDirection = 1;
        } else if (strcasecmp(player_move, "exit") == 0) {
            break;
        }

        // Evaluate player input
        if (validDirection) {
            if (newRoomIndex >= 0) {
                currentRoomIndex = newRoomIndex;
                steps++;  // Increment number of steps

                // Handle event in the new room
                if (rooms[currentRoomIndex].eventIndex >= 0) {
                    bonus += events[rooms[currentRoomIndex].eventIndex].effect;
                }
                break; // Valid move, exit the loop
            } else {
                mvprintw(9, 0, "There is no exit in that direction. Try again.");
                refresh();
                getch(); // Pause until the player presses a key
            }
        } else {
            mvprintw(9, 0, "Invalid command. Try again.");
            refresh();
            getch(); // Pause until the player presses a key
        }
    }
}

// Função para calcular o score do jogador com base no tempo restante, bônus e passos
void calculateScore(int remaining_time) {
    int score = remaining_time * 10 + bonus - steps * 5;
	
	mvprintw(4, 0, "YOUR SCORE: \n");
    mvprintw(6, 0, "SCORE: %d\n", score);
    mvprintw(7, 0, "Remaining Time Bonus: %d seconds * 10 = %d\n", remaining_time, remaining_time * 10);
    mvprintw(8, 0, "Events Bonus: +%d\n", bonus);
    mvprintw(9, 0, "Steps Penalty: %d steps * 5 = %d\n", steps, steps * 5);
}

// Geração de personagem
void generateCharacter() {
    clear();
    mvprintw(0, 0, "What's your name?");
	mvprintw(1, 0, "> ");
    refresh();
    scanw("%9s", name); 

    mvprintw(3, 0, "After some exploration in the dungeon, %s is coming back to the surface.\n"
					"For their bad luck, a powerful monster wakes! %s now have to run for\n"
					"their life! Will they be able to find their way out of this Labyrinth? \n", name, name);
	refresh();
	mvprintw(8, 0, "Press ENTER to start!");
    refresh();
    getch();
}