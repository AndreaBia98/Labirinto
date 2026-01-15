# Risoluzione Labirinto :  Labirinto_struct

## Descrizione
- Programma C (compilabile in Visual Studio) che carica un labirinto da `maze.json` e simula una persona che lo percorre rispettando una semplice regola di scelta della direzione (priorità: destra, avanti, sinistra, indietro).
- Visualizzazione su console con simboli grafici (muri, frecce, uscita).

### File principali
- `Labirinto_struct.cpp` — codice sorgente.
- `maze.json` — labirinto di input (formato JSON, vedi sezione sotto).
- file di progetto Visual Studio: `Labirinto_struct.vcxproj*`.

### Requisiti
- Windows (usa API console e `Sleep`).
- Visual Studio 2022/2025/2026 (o toolchain compatibile per codice C).
- Console che supporti la pagina di codici OEM (per la corretta visualizzazione dei caratteri ASCII estesi).

### Come buildare
1. Apri la soluzione o il progetto in Visual Studio.
2. Seleziona la configurazione desiderata (__Debug__ o __Release__) e la piattaforma (es. x86).
3. Compila con __Build > Build Solution__ oppure premi __Ctrl+Shift+B__.

###Esecuzione
- Esegui dal Visual Studio con __Debug > Start Without Debugging__ (o __Ctrl+F5__) per vedere la console rimanere aperta.
- Il programma carica `maze.json` dalla cartella corrente dell'eseguibile; assicurati che il file sia copiato nella stessa directory dell'eseguibile (Impostazione progetto: __General > Output Directory__ o proprietà file -> __Copy to Output Directory__ = "Copy if newer").

## Formato di `maze.json`
- Deve contenere una proprietà `maze` che è un array di righe; ogni riga è un array di singole stringhe contenenti un singolo carattere:
  - Caratteri supportati:
    - `"X"` — muro (perimetro obbligatorio tranne per S/E)
    - `" "` — spazio percorribile
    - `"S"` — start (esattamente 1)
    - `"E"` — exit (esattamente 1)
- Tutte le righe devono avere la stessa lunghezza (stesso numero di colonne).
- Il perimetro del labirinto (prima/ultima riga, prima/ultima colonna) deve contenere solo `X`, `S` o `E`.
- Esempio (estratto da `maze.json` nel progetto):
  {
    "maze": [
      [ "X", "X", "X", ... ],
      ...
      [ "X", "S", "X", ..., "E", "X" ]
    ]
  }

## Comportamento del programma
- Funzioni chiave:
  - `load_maze` — legge `maze.json` e costruisce dinamicamente la matrice di char.
  - `validate_maze` — controlla perimetro, unicità di `S` e `E`, consistenza delle colonne.
  - `print_maze` — disegna la griglia su console; la persona è mostrata con freccia (ASCII 30/16/31/17), la uscita con ASCII 15, i muri con ASCII 177.
  - `move_person` — regola di movimento: prova destra -> avanti -> sinistra -> indietro.
  - `simulation_loop` — loop principale: stampa, mette in pausa 1s (`Sleep(1000)`) tra le mosse, termina quando: raggiunge `E`, torna a `S` (senza via d'uscita), o resta bloccata.
- Messaggi sullo stato vengono stampati su console (es: "USCITA TROVATA", "BLOCCATO", "NESSUNA VIA D'USCITA").

### Note tecniche e avvertenze
- Il codice gestisce la memoria dinamica manualmente; `free_maze` dealloca le righe e la matrice.
- In caso di errore di I/O o di allocazione, il programma chiama `error_and_exit` terminando con messaggio.
- Se i caratteri grafici non appaiono correttamente nella console, prova ad eseguire `chcp 437` nella console o imposta la proprietà del terminale su una font compatibile (es. Raster Fonts) prima di lanciare l'eseguibile.
- Il parser del JSON è rudimentale: estrae i singoli caratteri tra virgolette riga per riga; mantenere il formato semplice come nell'esempio.

### Possibili miglioramenti
- Usare un parser JSON reale (es. cJSON) per robustezza.
- Migliorare la compatibilità della console (Unicode) e rimuovere dipendenza da codici ASCII OEM.
- Aggiungere opzioni CLI per specificare il file maze, velocità simulazione, modalità passo-passo.

## Licenza
- In assenza di indicazioni, consideralo codice di esempio; aggiungi una licenza esplicita se necessario (es. MIT).

Contatti / Note finali
- Apri issue o modifica `maze.json` per testare nuovi labirinti. Buona sperimentazione.
