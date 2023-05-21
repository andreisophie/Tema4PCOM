# Tema4PCOM

made by Andrei Mărunțiș

## Funcționalitatea clientului

Clientul implementeaza **toate** funcționalitatile mentionate in tema: login, logout, enter_library, get/add/delete book, get books. Aceste comenzi pot fi combinate, clientul le va interpreta corect si va afisa mesaje sugestive catre utilizator.

In functie de comanda primita de client, acesta fie o va executa, trimitand o cerere catre server, fie va refuza si nu va trimite nicio cerere catre server:

- Comanda de `login` va fi executata mereu; clientul va cere utilizatorului sa introduca nume de utilizator si parola, iar in functie de raspunsul primit de la server, va afisa un mesaj de succes (caz in care va salva cookie-ul de conectare) sau un mesaj de eroare
- Orice alta comanda poate sa esueze daca utilizatorul nu are permisiuni suficiente; spre exemplu:
    - daca utilizatorul incearca sa execute `enter_library` fara sa fi dat `login` in prealabil, clientul va afisa un mesaj de eroare si nici macar nu va mai trimite cerere catre server (fiind o certitudine ca va primi o eroare de tipul "user is not logged in").
    - daca utilizatorul incearca sa execute orice comanda legata de biblioteca (alta decat `enter_library`) fara sa fi dat `enter_library` in prealabil, clientul va afisa un mesaj de eroare si nu va trimite nicio cerere catre server, pe acelasi fundament ca si mai sus

## Stocarea cookie-urilor si token-ului

Cookie-ul de sesiune si token-ul de acces la biblioteca sunt stocate in doua variabile globale si sunt alocate dinamic. Atunci cand user-ul nu este logat si/ sau nu a acceesat biblioteca, acesti doua variabile au valoarea `NULL` pentru a fi usor de identificat daca un user-ul este logat.

Atunci cand un user se logheaza, clientul extrage automat din raspunsul server-ului cookie-ul de sesiune (daca a primit un cod de succes), alocand memorie pentru acel cookie si copiind-ul in variabila corespunzatoare. Exact acelasi proces se executa pentru token-ul de acces si comanda `enter_library`.

De mentionat ca mereu cookie-ul de sesiune este primul completat, apoi token-ul de securitate (deoarece comanda `enter_library` trebuie mereu precedata de o comanda `login`). Astfel, functiile care au nevoie de acces la biblioteca (spre exemplu `get_books`) vor verifica numai daca token-ul este alocat, intrucat cookie-ul de sesiune este mereu alocat si el daca exista token-ul.

Comanda de logout va elibera ambele zone de memorie (daca ele sunt ocupare), trimitand si o cerere catre server, dupa caz.

## Parsarea raspunsurilor primite de la server

Codul raspunsului primit de la server este mereu in dreptul celui de-al 10-lea caracter din string-ul primit. Astfel, codul de succes/ eroare este usor de extras. In functie de codul de raspuns primit de la server, execut anumite bucati de cod corespunzatoare fiecarei functii (spre exemplu, afisez mesaj de eroare/ succes, extrag date precum cookie-ul de sesiune din raspuns etc.).

Cookie-ul de sesiune este mereu precedat de `Set-Cookie`, astfel ca pentru a il gasi caut acest string (folosind functia de biblioteca `strstr`) si apoi il copiez in variabila globala destinata.

Pentru token-ul de acces trebuie parsat un JSON; pentru asta folosesc biblioteca parson.

## Alte observatii

Pentru a valida input-urile numerice primite de la tastatura, le citesc sub forma de string, verific ca acel string are formatul unui numar, iar daca gasesc litere/ simboluri/ alte caractere care nu sunt numere, afisez un mesaj de eroare si cer utilizatorului sa introduca din nou o valoare numerica.

String-urile citite de la tastura (folosind functia `fgets`) pot avea caractere speciale la final (spre exemplu newline `\n`). folosesc o functie speciala pentru a sterge caracterele respective, de care nu am nevoie.

Rezolvarea temei a fost in continuarea codului scris la laboratorul de HTTP; functiile rezolvate la momentul respectiv au fost extinse astfel:

- Functiile de construit request HTTP de tip GET si POST au primit un parametru suplimentar, token-ul de securitate, care va fi inclus in cerere daca este prezent
- Functia de construit request HTTP de tip DELETE este asemanatoare cu functia de trimis cereri de tip POST.

Chestii care mi-au placut la tema:

- Enunt clar, bine organizat, cu regulile importante highlight-uite
- Nivel de dificultate potrivit (dupa celelalte teme la materia PCom care erau absolut cancer, tema asta a fost abordabila si consider ca am invatat din ea la fel de multe ca din primele doua teme, daca nu mai mult)
- Lipsa unui checker face ca anumite output-uri sa fie la alegerea noastra (mesaje de eroare, formatul afisarii datelor etc.) ceea ce a fost foarte nice, nu a trebuit sa ma chiorasc in enunt/teste si, de asemenea, enuntul temei a fost mult mai lightweight, intrucat nu apareau mesajele de eroare
- De altfel, forma temei (enunt clar explicat, concis dar nici cu prea multe explicatii, lipsa unui checker) a incurajat skill-ul de "critical thinking"/ problem solving, necesare la un job real
- Lucrul cu o librarie externa prost documentata (parson) m-a invatat cum sa tratez aceste situatii care apar des la locul de munca 😄
- Overall tema mea preferata din acest semestru