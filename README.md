# universe-ball

### Opis projekta:

Projekat predstavlja igricu u kojoj se loptica (planetica) odbija od zidove,a mi joj
uz pomoc plocice ne damo da propadne u svemir. Ukoliko se to desi igra je zavrsena,
a rezultat predstavlja broj uspesnih "hvatanja" loptice odnosno odbijanja loptice od 
plocicu.

### Kako instalirati i pokrenuti:
1. Prvo instalirati GLUT biblioteku.
  ```
    apt-get install freeglut3
  ```
2. Klonirati repozitorijum.
  ```
    git clone https://github.com/mi16401/universe-ball.git
  ```
3. Kompilacija:
  ```
    make
  ```
5. Pokretanje:
  ```
    ./project
  ```
### Kontrole (nije vazno da li je slovo malo ili veliko):
| **taster** | **akcija** |
| :---:  | :---: |
| 's' | start - pokretanje igrice |
| 'p' | pause - pauza |
| 'r' | reset - vracanje igre koja nije zavrsena na pocetak |
| 'n' | new game - ukoliko je igra zavrsena moze se zapoceti nova (n + s) |
| '->' | pomeranje plocice u levo |
| '<-' | pomeranje plocice u desno |
| 'Esc' | izlazak iz programa |

### Autor:
Tamara Garibovic

#### Napomena: Projekat radjen 2017. godine.