# Labirinto Simulator

Simulazione di una persona che attraversa un labirinto seguendo la **regola della mano destra**, implementata in **C**.

Il risultato richiesto si trova all interno del file zip **risultato.zip**.
i file all interno sono:
- Labirinto_struct.exe: eseguibile del programma
- File per la corretto funzionamento: cjson.dll e maze.json
- start.bat : file batch per la sua esecuzione. 

la medesima cartella si trova all interno di : " Labirinto_struct/x64/Release "

---

## Descrizione

Il programma simula il movimento di una persona all’interno di un labirinto caricato da file JSON.  
La simulazione è visualizzata su console ed evolve turno per turno con un intervallo di 1 secondo.

L’obiettivo è trovare l’uscita (`E`) partendo dallo start (`S`).  
Se la persona ritorna allo start, il labirinto non ha una via di uscita valida.


## Modalità di lavoro
Il programma è stato sviluppato in visualcode studio, implementando il linguaggio di programmazione c. 

- Per eseguire su terminale di windows è stata implementata la libreria windows.h per `sleep()`
- Per accedere al file JSON è stata utilizzata la libreria [cJSON](https://github.com/DaveGamble/cJSON):
    - `cJSON_Parse(buffer)` per convertire il contenuto del file JSON in una struttura dati navigabile.
    - `cJSON_GetObjectItem(root, "maze")` per accedere al campo `"maze"` del file.
    - `cJSON_IsArray()` per verificare che il labirinto e le sue righe siano array validi.
    - `cJSON_GetArraySize()` per determinare il numero di righe e colonne del labirinto.
    - `cJSON_GetArrayItem()` per scorrere righe e celle del labirinto.
    - `cJSON_GetStringValue()` per leggere il contenuto delle singole celle.
    - `cJSON_Delete()` per liberare la memoria allocata dalla libreria al termine del parsing.


## Requisiti

- Ambiente a riga di comando su Windows:
  - Prompt dei comandi (cmd.exe)
  - PowerShell
  - Terminale integrato di Visual Studio

---

## Strutture dati

### Labirinto

Contiene:
- matrice con dimenzione dinamica del labirinto
- numero di righe e colonne
- coordinate di start ed end

### Persona

Contiene:
- posizione corrente 
- direzione corrente
- nome 

---

## Validazione del labirinto

Prima dell’avvio della simulazione vengono verificate le seguenti regole:

1. Tutte le righe devono avere lo stesso numero di colonne
2. Il perimetro può contenere solo:
   - `X` (muro)
   - `S` (start)
   - `E` (end)
3. Deve esistere **un solo start**
4. Deve esistere **un solo end**
5. Start ed end devono trovarsi sul perimetro

In caso di errore il programma termina immediatamente con un messaggio esplicativo.

---

## Formato del file `maze.json`

```json
{
  "maze": [
    ["X","X","X","X","X"],
    ["X","S"," ","E","X"],
    ["X","X","X","X","X"]
  ]
}
```

### Legenda

* `X` : muro
* `S` : start
* `E` : end
* ` ` : spazio percorribile

---

## Visualizzazione su console

* I muri sono rappresentati con blocchi ASCII (▒)
* L’uscita è evidenziata con un simbolo speciale (un sole ☼ )
* La persona è rappresentata da una freccia direzionale

| Direzione | Simbolo |
| --- | --- |
| Nord | `^` |
| Est | `>` |
| Sud | `v` |
| Ovest | `<` |

---

## Logica di movimento

La persona applica la **regola della mano destra** con la seguente priorità:

1. Destra
2. Avanti
3. Sinistra
4. Indietro

Se nessuna mossa è possibile, la simulazione termina.



## Note

* Codice modulare e commentato
* Gestione corretta della memoria dinamica
* Progetto facilmente estendibile, grazie ad uso di strutture e funzioni adeguate

---
