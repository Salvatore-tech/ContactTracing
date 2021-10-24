# ContactTracing
ContactTracing è un progetto di reti di calcolatori, basato sull'utilizzo delle socket in linguaggio C che ha lo scopo di individuare il tracciamento dei contatti. Presenta un'architettura peer to peer in cui ogni peer è identificato attraverso un id alfanumerico di 64 byte. Per entrare nella rete ogni peer deve registrarsi presso un server, che gestisce le richieste inoltrate dai peer. I dati che i peer scambiano con il server o i restanti peer sono rappresentati da pacchetti di diverso tipo, che variano a seconda delle azioni da eseguire. In tutti i casi però, i pacchetti sono formati da un header che definisce il tipo di messaggio, e da un body che può essere un indirizzo, un id o altro. Ad intervalli di tempo regolari, il peer contatta i peer raggiungibili, scambiando con essi gli id alfanumerici precedentemente generati. I contatti avvenuti sono salvati da ambo i peer coinvolti. Il server, ha l'ulteriore compito di segnalare tramite un messaggio, i peer positivi/infetti. Quest'ultimo procederà ad informare i peer della rete riguardo la propria condizione di positività. Ciò è realizzato attraverso una trasmissione broadcast dove gli infetti inviano la lista dei peer con cui sono stati in contatto. I peer che sono in ascolto sulla rete riceveranno quindi tale lista e controlleranno se vi figurano. In caso affermativo, sarà mostrato un messaggio all'utente.

# Istruzioni per la compilazione
E' stato utilizzato il build tool CMake, che ricordiamo utilizza scripts chiamati
CMakeLists per generare gli eseguibili per uno specifico environment (makefles
su macchine Unix ad esempio). Nella project root è presente un file CMakeLists.txt che contiene il nome del progetto, librerie da linkare (-pthread.h) e la
lista dei sorgenti da buildare.
Il progetto è stato buildato spostandosi nel path ./cmake-build-debug e lanciando da terminale il comando make.
Tuttavia, generalmente, il Makefile generato su una macchina non è portabile.
Per ovviare a ciò, nella project root è stato creato uno script (buildWithGcc.sh) che utilizza gcc.

# Istruzioni per l'esecuzione
Dopo la fase di compilazione, si procede, innanzitutto, ad avviare il server e
successivamente uno o più peer. E' possibile utilizzare lo script precedente, indicando, per l'esecuzione del peer, il numero di istanze desiderate.
Il server mostrerà un messaggio, restando in attesa di richieste da parte dei peer.
Al lancio dei peer, come precedentemente illustrato, essi invieranno un messaggio al server per entrare a far parte della rete e il server mostrerà a video l'arrivo
dei peer.
Quest'ultimi resteranno in attesa di un input da parte dell'utente che dovrà
inserire una delle opzioni mostrate dal menù di scelta.

## Credits
- pizza11x
- antoniocolucci
