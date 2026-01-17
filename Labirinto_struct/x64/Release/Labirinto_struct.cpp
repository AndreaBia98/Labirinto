#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include <cjson/cJSON.h>
// ===================== COSTANTI =====================

#define NORD 0
#define EST 1
#define SUD 2
#define OVEST 3

const char* frecce[] = { "^", ">", "v", "<" };            // rappresentazione testuale delle direzioni: N E S O

// ===================== STRUTTURE =====================

typedef struct {
    char** maze;
    int rows;
    int cols;
    int start_r, start_c;
    int end_r, end_c;
} Labirinto;


void init_labirinto(Labirinto* lab) {
    if (!lab) return;                                     // controllo che il puntatore al labirinto sia valido

    lab->maze = NULL;                                     // nessuna memoria allocata per il labirinto
    lab->rows = 0;                                        // inizializzo numero righe a 0
    lab->cols = 0;                                        // inizializzo numero colonne a 0
    lab->start_r = -1;                                    // posizione start non ancora definita
    lab->start_c = -1;
    lab->end_r = -1;                                      // posizione end non ancora definita
    lab->end_c = -1;
}

typedef struct {
    int r, c;
    int dir;
    char name[32];                                       
} Persona;

void init_persona(Persona* p) {
    if (!p) return;

    p->r = -1;
    p->c = -1;
    p->dir = EST;

	strncpy_s(p->name, sizeof(p->name), "Andrea", _TRUNCATE);       // io mi chiamo Andrea!!!
}



// ===================== UTILS =====================

void error_and_exit(const char* msg) {

    printf("ERRORE: %s\n", msg);
    exit(1);
}

// ===================== LABIRINTO =====================



void free_maze(Labirinto* lab) {
    if (!lab) return;                                   // controllo che il puntatore al labirinto sia valido
    if (!lab->maze) return;                             // se il labirinto non è stato allocato non c'è nulla da liberare

    for (int i = 0; i < lab->rows; i++) {               // loop su tutte le righe del labirinto
        free(lab->maze[i]);                             // libero la singola riga (array di char)
    }

    free(lab->maze);                                   

    lab->maze = NULL;                                   // evito dangling pointer
    lab->rows = 0;                                      // resetto dimensioni
    lab->cols = 0;
}


void load_maze(Labirinto* lab, const char* filename) {
    if (!lab || !filename)                                // controllo validità parametri di ingresso
        error_and_exit("Parametro non valido per load_maze");

    FILE* f = NULL;
    if (fopen_s(&f, filename, "rb") != 0 || !f)           // apertura file JSON in modalità binaria
        error_and_exit("Impossibile aprire file");

    // ===================== LETTURA FILE =====================

    if (fseek(f, 0, SEEK_END) != 0) {                      // sposto il cursore a fine file
        fclose(f);
        error_and_exit("Errore lettura file");
    }

    long len = ftell(f);                                  // ottengo dimensione totale del file
    if (len <= 0) {                                       // file vuoto o non valido
        fclose(f);
        error_and_exit("File JSON vuoto o non leggibile");
    }

    rewind(f);                                            // riporto il cursore all'inizio del file

    char* buffer = (char*)malloc((size_t)len + 1);        // alloco buffer per contenere tutto il JSON
    if (!buffer) {                                        // controllo allocazione memoria
        fclose(f);
        error_and_exit("Memoria insufficiente per buffer JSON");
    }

    if (fread(buffer, 1, (size_t)len, f) != (size_t)len) {// leggo tutto il file nel buffer
        free(buffer);
        fclose(f);
        error_and_exit("Errore lettura file JSON");
    }

    buffer[len] = '\0';                                   // aggiungo terminatore di stringa
    fclose(f);                                            // chiudo il file (non serve più)

    // ===================== PARSING JSON =====================

    cJSON* root = cJSON_Parse(buffer);                    // parsing del contenuto JSON
    free(buffer);                                         // buffer non più necessario

    if (!root)                                            // controllo esito parsing
        error_and_exit("Errore nel parsing del JSON");

    cJSON* maze = cJSON_GetObjectItem(root, "maze");       // estraggo il campo "maze"
    if (!cJSON_IsArray(maze)) {                            // controllo che sia un array
        cJSON_Delete(root);
        error_and_exit("Campo 'maze' mancante o non array");
    }

    int rows = cJSON_GetArraySize(maze);                   // numero di righe del labirinto
    if (rows <= 0) {                                      // labirinto vuoto non ammesso
        cJSON_Delete(root);
        error_and_exit("Labirinto vuoto nel JSON");
    }

    // ===================== INIZIALIZZAZIONE STRUTTURA =====================

    lab->rows = rows;                                     // salvo numero di righe
    lab->cols = 0;                                        // inizializzo colonne (definite dalla prima riga)

    lab->maze = (char**)malloc(rows * sizeof(char*));     // alloco array di puntatori alle righe
    if (!lab->maze) {                                     // controllo allocazione
        cJSON_Delete(root);
        error_and_exit("Memoria insufficiente per lab->maze");
    }

    // ===================== LETTURA RIGHE =====================

    for (int i = 0; i < rows; i++) {                       // loop su tutte le righe del labirinto
        cJSON* row = cJSON_GetArrayItem(maze, i);          // estraggo la riga i-esima dal JSON
        if (!cJSON_IsArray(row)) {                         // controllo che la riga sia un array
            free_maze(lab);
            cJSON_Delete(root);
            error_and_exit("Riga del labirinto non valida");
        }

        int cols = cJSON_GetArraySize(row);                // numero di colonne della riga corrente
        if (cols <= 0) {                                  // riga vuota non ammessa
            free_maze(lab);
            cJSON_Delete(root);
            error_and_exit("Riga vuota nel labirinto");
        }

        if (i == 0)
            lab->cols = cols;                              // la prima riga definisce il numero di colonne
        else if (cols != lab->cols) {                      // controllo uniformità colonne
            free_maze(lab);
            cJSON_Delete(root);
            error_and_exit("Righe con numero di colonne diverso");
        }

        lab->maze[i] = (char*)malloc(cols + 1);            // alloco memoria per la riga + '\0'
        if (!lab->maze[i]) {                               // controllo allocazione
            free_maze(lab);
            cJSON_Delete(root);
            error_and_exit("Memoria insufficiente per riga labirinto");
        }

        // ===================== LETTURA CELLE =====================

        for (int j = 0; j < cols; j++) {                   // loop su tutte le colonne della riga
            cJSON* cell = cJSON_GetArrayItem(row, j);      // estraggo la singola cella
            const char* val = cJSON_GetStringValue(cell);  // leggo il valore stringa della cella

            if (!val || val[0] == '\0') {                  // controllo che la cella sia valida
                free_maze(lab);
                cJSON_Delete(root);
                error_and_exit("Cella non valida nel labirinto");
            }

            lab->maze[i][j] = val[0];                      // salvo il carattere nel labirinto
        }

        lab->maze[i][lab->cols] = '\0';                    // terminatore di stringa per sicurezza/debug
    }

    cJSON_Delete(root);                                    // libero struttura JSON
}


void validate_maze(Labirinto* lab) {


    if (!lab) error_and_exit("Parametro non valido per validate_maze");


    if (lab->rows == 0 || lab->cols == 0)
        error_and_exit("Labirinto vuoto");

    int start_count = 0, end_count = 0;

    for (int i = 0; i < lab->rows; i++) {                    // loop su tutte le righe
        for (int j = 0; j < lab->cols; j++) {                // loop su tutte le colonne
            char cell = lab->maze[i][j];                     // cella corrente

            if (cell == 'S') {                               // se trovo la cella di start
                start_count++;                               // incremento il contatore degli start
                lab->start_r = i;                            // salvo posizione start
                lab->start_c = j;
            }

            if (cell == 'E') {                               // se trovo la cella di end
                end_count++;                                 // incremento il contatore degli end
                lab->end_r = i;                              // salvo posizione end
                lab->end_c = j;
            }

            if (i == 0 || i == lab->rows - 1 ||              // controllo se sono sul perimetro
                j == 0 || j == lab->cols - 1) {

                if (cell != 'X' && cell != 'S' && cell != 'E') { // sul perimetro sono ammessi solo X, S o E
                    free_maze(lab);
                    error_and_exit("Perimetro non valido");
                }
            }
        }
    }

    if (start_count != 1 || end_count != 1) {
        free_maze(lab);
        error_and_exit("Start o End mancanti o duplicati");
    }


}

void print_maze(Labirinto* lab, Persona* p) {
    if (!lab || !p) return;                               // controllo che i puntatori a labirinto e persona siano validi

    system("cls");                                        // pulisce la console prima di ridisegnare il labirinto

    for (int i = 0; i < lab->rows; i++) {                 // loop su le righe del labirinto
        for (int j = 0; j < lab->cols; j++) {             // loop su le colonne del labirinto

            if (i == p->r && j == p->c) {                 // se la cella corrente è occupata dalla persona
                int dir = (p->dir >= 0 && p->dir < 4)               // controllo che la direzione sia valida
                    ? p->dir                     // uso la direzione corrente
                    : EST;                       // fallback di sicurezza
                printf("%s", frecce[dir]);                // stampa la freccia che indica la direzione della persona
            }
            else if (i == lab->end_r && j == lab->end_c)  // se la cella corrente è l'uscita 'E'
                printf("%c", (unsigned char)15);          // stampa un simbolo speciale per evidenziare l'uscita
            else if (lab->maze[i][j] == 'X')              // se la cella corrente è un muro
                printf("%c", (unsigned char)177);         // stampa un blocco ASCII per rappresentare il muro
            else
                printf("%c", lab->maze[i][j]);            // stampa il contenuto originale della cella (spazio o altro)
        }

        printf("\n");                                     // vai a capo dopo aver stampato una riga completa
    }
}

// ===================== MOVIMENTO =====================

void delta(int d, int* dr, int* dc) {
    if (!dr || !dc) return;
    switch (d) {
    case NORD:  *dr = -1; *dc = 0; break;
    case EST:   *dr = 0;  *dc = 1; break;
    case SUD:   *dr = 1;  *dc = 0; break;
    case OVEST: *dr = 0;  *dc = -1; break;
    default:    *dr = 0;  *dc = 0; break;
    }
}

int turn_right(int d) { return (d + 1) % 4; }
int turn_left(int d) { return (d + 3) % 4; }
int turn_back(int d) { return (d + 2) % 4; }

int can_move(Labirinto* lab, Persona* p, int d) {
    if (!lab || !p) return 0;
    int dr, dc;
    delta(d, &dr, &dc);

    int nr = p->r + dr;
    int nc = p->c + dc;

	if (nr < 0 || nr >= lab->rows || nc < 0 || nc >= lab->cols) // controllo uscita dai limiti verticali (nr) e orizzontali (nc)                             
        return 0;                                             // movimento non consentito

    if (lab->maze[nr][nc] == 'X')                             // se la cella di destinazione è un muro
        return 0;                                             // movimento bloccato

    return 1;                                                 // movimento consentito

}

// restituisce 1 se la persona si è mossa, 0 se bloccata (nessuna mossa possibile)
int move_person(Labirinto* lab, Persona* p) {
    if (!lab || !p) return 0;

    int right   = turn_right(p->dir);
    int left    = turn_left(p->dir);
    int back    = turn_back(p->dir);

    int chosen = -1;
	// selezione della direzione secondo le priorità destra, avanti, sinistra, indietro, se nessuna possibile la scelta -1
    if (can_move(lab, p, right))       chosen = right;        
    else if (can_move(lab, p, p->dir)) chosen = p->dir;      
    else if (can_move(lab, p, left))   chosen = left;        
    else if (can_move(lab, p, back))   chosen = back;       
    else                               chosen = -1;        

	if (chosen < 0) return 0;                                 // se mossa impossibile (-1) esco

    p->dir = chosen;                                          // aggiorno la direzione della persona
    int dr, dc;
    delta(p->dir, &dr, &dc);                                  // calcolo spostamento riga/colonna
    p->r += dr;                                               // aggiorno posizione riga
    p->c += dc;                                               // aggiorno posizione colonna


    return 1;
}

// ===================== SIMULAZIONE =====================

void simulation_loop(Labirinto* lab, Persona* p) {
    if (!lab || !p) return;

    p->r = lab->start_r;                                    // Il punto di partenza della persona è la cella 'S' del labirinto
    p->c = lab->start_c;
    //p->dir = EST;                                           // direzione iniziale arbitraria (EST)

    if (lab->start_r == lab->rows - 1)        // S è sull'ultima riga (basso) -> guardare verso l'alto
        p->dir = NORD;
    else if (lab->start_r == 0)               // S è sulla prima riga (alto) -> guardare verso il basso
        p->dir = SUD;
    else if (lab->start_c == 0)               // S è sulla prima colonna (sinistra) -> guardare verso destra
        p->dir = EST;
    else if (lab->start_c == lab->cols - 1)   // S è sull'ultima colonna (destra) -> guardare verso sinistra
        p->dir = OVEST;
    else                                      // caso generico: direzione di default
        p->dir = EST;


	
    print_maze(lab, p);                                     // stampa iniziale del labirinto con la posizione iniziale della persona
    Sleep(1000);

    while (true){
        int moved = move_person(lab, p);                    // movimento della persona
        if (!moved) {
            print_maze(lab, p);
            printf("\nBLOCCATO: nessuna mossa possibile!\n");
            break;
        }

        print_maze(lab, p);                                 // stampa del labirinto con la posizione aggiornata della persona

        if (p->r == lab->end_r && p->c == lab->end_c) {     // controllo se la persona ha raggiunto l'uscita 'E'
            printf("\nUSCITA TROVATA!\n");
            break;
        }

        if (p->r == lab->start_r && p->c == lab->start_c) { // controllo se la persona è tornata al punto di partenza 'S'
            printf("\nNESSUNA VIA D'USCITA!\n");
            break;
        }

        Sleep(1000);                                        // aspetto 1 secondo (1000 ms su windows)
    }
}

// ===================== MAIN =====================

int main() {
    Labirinto maze;
    Persona p;


    init_labirinto(&maze);                                     // inizializza la struttura labirinto
    init_persona(&p);                                     // inizializza la struttura labirinto


    load_maze(&maze, "maze.json");                           // carica il labirinto da file

    validate_maze(&maze);                                    // validazione il labirinto
    print_maze(&maze, &p);
    printf("\n premi per partenza simulazione\n");
    getchar();
    simulation_loop(&maze, &p);                              // esegue la simulazione del movimento della persona nel labirinto
    free_maze(&maze);                                         // libera la memoria allocata per il labirinto   

    printf("\nFine Programma\n");
    getchar();
    return 0;
}
