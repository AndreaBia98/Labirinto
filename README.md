# Labirinto Simulator

Simulazione di una persona che attraversa un labirinto seguendo la **regola della mano destra**, implementata in **C**.

Il risultato richiesto si trova all intenrno del file zip **risultato.zip**
la medesima cartella si trova all interno di : " Labirinto_struct/x64/Release "

---

## Descrizione

Il programma simula il movimento di una persona all’interno di un labirinto caricato da file JSON.  
La simulazione è visualizzata su console ed evolve turno per turno con un intervallo di 1 secondo.

L’obiettivo è trovare l’uscita (`E`) partendo dallo start (`S`).  
Se la persona ritorna allo start, il labirinto non ha una via di uscita valida.


## Requisiti

- Windows
- Compilatore C (Visual Studio consigliato)
- Libreria **cJSON**
- Console testuale

---

## Strutture dati

### Labirinto

Contiene:
- matrice dinamica del labirinto
- numero di righe e colonne
- coordinate di start ed end

### Persona

Contiene:
- posizione corrente
- direzione corrente
- nome (easter egg: `Andrea`)

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

* `X` → muro
* `S` → start
* `E` → end
* → spazio percorribile

---

## Visualizzazione su console

* I muri sono rappresentati con blocchi ASCII
* L’uscita è evidenziata con un simbolo speciale
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
* Progetto facilmente estendibile
* Presente un easter egg nella struttura `Persona`

---
