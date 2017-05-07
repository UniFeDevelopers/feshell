# Una shell #

## Progetto del corso di Laboratorio Sistemi Operativi a.a. 2016/2017 ##

### Introduzione ###

Lo scopo di questo progetto è creare una shell, che sia usabile e che sia in grado di supportare redirect e pipe. 
Quando opportuno deve essere in grado di eseguire comandi esterni ed invece, deve poter usare un "sano" default interno (che sarà dettagliato nel corso di questo documento).
Lo scopo principale del progetto è di familiarizzare lo studente con le strutture del filesystem, l’esecuzione di comandi dall’interno del listato C (con i conseguenti problemi di sicurezza), con il sistema, in generale.

### Descrizione del progetto ###

Lo studente in questo progetto scriverà una shell, nominata feshell.
La shell deve essere in grado di ricevere comandi, parsarli e capire cosa farne. Nel caso in cui il comando
non sia definito come “interno” la shell deve usare un opportuno comando della famiglia exec (si veda
‘man 3 exec’) ed eseguire in quell’ambiente il comando richiesto. L’unico comando da definire come
interno è ’ls’. In questo caso la shell scritta dallo studente non deve usare il binario tipicamente in
/usr/bin ma piuttosto reimplementare, come libreria linkata alla shell o come funzioni definite nel tree
della shell stessa, una propria versione di ls.

Non è importante che la versione della shell e di ls realizzate dallo studente siano in grado di supportare tutte le opzioni che bash ed ls sono in grado di supportare.

*feshell* deve essere in grado di supportare la redirezione (<, > e ») e le pipe (|).
Questo supporto deve essere sostenuto anche in casi misti: e.g. `ps aux | grep ciccio > processiDiCiccio`

## Installazione ##

```
#!bash

git clone https://github.com/feshellWithLove/feshell.git

cd feshell/
mkdir bin/

make
./bin/feshell
```
