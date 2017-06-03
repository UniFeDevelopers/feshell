# Una shell #

## Progetto del corso di Laboratorio Sistemi Operativi a.a. 2016/2017 ##

### Introduzione ###

Lo scopo di questo progetto è creare una shell, che sia usabile e che sia in grado di supportare redirect e pipe.
Quando opportuno deve essere in grado di eseguire comandi esterni ed invece, deve poter usare un "sano" default interno (che sarà dettagliato nel corso di questo documento).
Lo scopo principale del progetto è di familiarizzare lo studente con le strutture del filesystem, l'esecuzione di comandi dall'interno del listato C (con i conseguenti problemi di sicurezza), con il sistema, in generale.

### Descrizione del progetto ###

Lo studente in questo progetto scriverà una shell, nominata feshell.
La shell deve essere in grado di ricevere comandi, parsarli e capire cosa farne. Nel caso in cui il comando
non sia definito come "interno" la shell deve usare un opportuno comando della famiglia exec (si veda
`man 3 exec`) ed eseguire in quell'ambiente il comando richiesto. L’unico comando da definire come
interno è `ls`. In questo caso la shell scritta dallo studente non deve usare il binario tipicamente in
`/usr/bin` ma piuttosto reimplementare, come libreria linkata alla shell o come funzioni definite nel tree
della shell stessa, una propria versione di ls.

Non è importante che la versione della shell e di ls realizzate dallo studente siano in grado di supportare tutte le opzioni che bash ed ls sono in grado di supportare. 

Per alcune opzioni, però è necessario che lo
studente scriva il codice necessario. Queste sono:
1. l’equivalente della opzione `-l` di ls canonico. Questo mostra tutte le informazioni relative ai file
(permessi, data di ultimo accesso o modifica, owner e gruppo, nome)
2. l’equivalente della opzione `-a` di ls canonico. Questo mostra anche i file normalmente nascosti (e
pertanto la versione dello studente di ls deve rispettare la convenzione secondo la quale i file che
iniziano con un punto (.) non devono altrimenti essere mostrati)
3. l’equivalente della opzione `-t` di ls canonico. Questa opzione ordina i file/directory nel listato
secondo il tempo di creazione/modifica: dal più recente al più vecchio
4. l’equivalente della opzione `-h` di ls canonico. Questa opzione usa suffissi per le unità: KB per
KiloByte, MB per MegaByte, ecc. e questo dovrebbe diminuire la lunghezza in cifre del numero
mostrato per la dimensione a 3 caratteri o meno.

Inoltre, la colorazione dell’output è opzionale (per esempio un colore per i file, secondo il tipo, ed un
colore diverso per le directory).

*feshell* deve essere in grado di supportare la redirezione (<, > e >>) e le pipe (|).
Questo supporto deve essere sostenuto anche in casi misti: e.g. `ps aux | grep ciccio > processiDiCiccio`

## Setup ##

```
git clone https://github.com/feshellWithLove/feshell.git

cd feshell/
mkdir bin/

make
./bin/feshell
```

### Compilazione automatizzata ###

Per la fase di sviluppo abbiamo automatizzato la compilazione con `grunt`.  
Viene lanciata al salvataggio dei file `.c` e `.h`.

Si può avviare con:
```
grunt
```


Per installare *grunt*, installare **npm** se non fosse installato (Debian/Ubuntu):
```
sudo apt-get install nodejs
```
e successivamente installare **grunt** e le sue dipendenze all'interno della directory principale del repository:
```
npm install
```

## Uso ##

La **shell** si comporta come bash, rispetta le specifiche del presente documento e si avvia con:
```
./bin/feshell
```

Per quanto riguarda **ls**, si veda la pagina di help per le opzioni disponibili:
```
ls --help
```
