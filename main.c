#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <GL/glut.h>
#include "image.h"

/*Ime fajla sa teksturom*/
#define FILENAME "universe.bmp"
/* Identifikator teksture. */
static GLuint names[1];

/*Makroi za timer.*/
#define TIMER_H
#define TIMER_ID 0
/*Definisemo kao globalnu promenljivu jer cemo vremenom ubrzavati animaciju*/
int TIMER_INTERVAL=50;

/*definisemo maksimalno i minimalno rastojanje izmedju lopte i plocice
koju smo odredili uz pomoc euklidkog rastojanja najblize i najudaljenije tacke
koja ispunjava uslov dodira sa plocicom.*/
#define MAX_RASTOJANJE (2.236067977)
#define MIN_RASTOJANJE 1

/*Ivica po kojoj se odredjuje kolizija sa zidovima*/
#define IVICA 10

/*Maksimalan broj karaktera koji se moze naci u nisci koja se ispisuje na ekran.*/
#define MAX_KARAKTERA 256

/*Indikator koji govori da li je animacija u toku ili ne*/
static int animation_ongoing;
/*Indikator koji govori da li je igra zavrsena (1) ili nije (0)*/
static int game_over = 0;
/*Indikator koji govori da li je igrac pritisnuo taster za pauzu (1) ili nije (0).*/
static int pause = 0;

/*koordinate loptice*/
float x_koordinata;
float y_koordinata;
/*koordinate vektora*/
float x_vektora;
float y_vektora;
/*koordinate plocice*/
float x_plocice;
float y_plocice;

/*brojac koji belezi koliko puta je uspesno "uhvacena" loptica*/
int pogodak = 0;

/*Deklaracija callback funkcija.*/
static void on_keyboard(unsigned char key, int x, int y);
static void on_reshape(int width, int height);
static void on_display(void);
void SpecialInput(int key, int x, int y);

static void on_timer(int value);

/*Deklaracija pomocnih funkcija.*/
static void initialize(void);
void nacrtaj_scenu(void);
float random_vector(void);

/*Funkcije za ispis odredjenih poruka na ekran u odredjenom trenutku.*/
void ispisGameOver(void);
void ispisPause(void);
void ispisTrenutniScore(void);


int main(int argc,char** argv){
	/* Inicijalizacija GLUT-a */
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

	/*Podesavanje prozora */
	glutInitWindowSize(500,500);
	glutInitWindowPosition(100,100);
	glutCreateWindow(argv[0]);

	glutKeyboardFunc(on_keyboard);
  glutReshapeFunc(on_reshape);
	glutDisplayFunc(on_display);
	glutSpecialFunc(SpecialInput);

	/*Pocetna inicijalizacija indikatora animacije.*/
  animation_ongoing=0;

	/* ---Inicijalizacija globalnih promenljivih--- */

	/*Inicijalizuje se seed za random brojeve*/
	srand(time(NULL));

	/*Hocemo da pocetna pozicija loptice bude Random odabrana*/
	x_koordinata=(int)((float)rand()/RAND_MAX*16)-8;
	y_koordinata=(int)((float)rand()/RAND_MAX*17)-8.5;

	/*Pocetna inicijalizacija vektora.*/
	x_vektora=0.25;
	y_vektora=0.25;

	/*Pocetna inicijalizacija koordinata plocice. Hocemo y fiksno, a x Random.*/
	x_plocice=(int)((float)rand()/RAND_MAX*16)-8;
	y_plocice=-9.5;

	/* Obavlja se OpenGL inicijalizacija. */
	initialize();

	/*Glavna petlja programa.*/
	glutMainLoop();

return 0;
}

/*F-ja preuzeta sa casa, koja vrsi OpenGL inicijalizaciju.
Potrebna za teksturu pozadine.*/
static void initialize(void)
{
    /* Objekat koji predstavlja teskturu ucitanu iz fajla. */
    Image * image;
    /* Postavlja se boja pozadine. */
    glClearColor(0, 0, 0, 0);
    /* Ukljucuje se testiranje z-koordinate piksela. */
    glEnable(GL_DEPTH_TEST);
    /* Ukljucuju se teksture. */
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV,
              GL_TEXTURE_ENV_MODE,
              GL_REPLACE);
    /*
     * Inicijalizuje se objekat koji ce sadrzati teksture ucitane iz
     * fajla.
     */
    image = image_init(0, 0);
    /* Kreira se tekstura. */
    image_read(image, FILENAME);
    /* Generisu se identifikatori teksture. */
    glGenTextures(1, names);
    glBindTexture(GL_TEXTURE_2D, names[0]);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 image->width, image->height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image->pixels);
    /* Unistava se objekat za citanje tekstura iz fajla. */
    image_done(image);

}

/*Callback funkcija kojoj cemo detetkotvati kretanje plocice na strelice.*/
void SpecialInput(int key, int x, int y){

  switch(key){
		/*Leva strelica pomera plocicu u levo, po x-osi za 1.*/
    case GLUT_KEY_LEFT:
			if(x_plocice>-IVICA+2){
				x_plocice-=1;
			}
  		glutPostRedisplay();
  		break;
		/*Desna strelica pomera plocicu u desno, po x-osi za 1.*/
    case GLUT_KEY_RIGHT:
			if(x_plocice<IVICA-2){
				x_plocice+=1;
			}
			glutPostRedisplay();
			break;
  }
}

/*Callback funkcija koja realizuje akcije pritiskanjem odredjenog dugmeta na tastaturi.*/
static void on_keyboard(unsigned char key, int x, int y)
{
    switch (key) {
    case 27:
        /* Zavrsava se program. */
				glDeleteTextures(1, names);
        exit(0);
        break;

    case 's':
    case 'S':
        /* Pokrece se animacija. */
        if (!animation_ongoing && !game_over) {
            glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
            animation_ongoing = 1;
        }
				/*Dodajemo kako bi se uklonio ispis za pauzu kada je animacija u toku.*/
				if(pause != 0)
					pause = 0;
        break;

    case 'p':
    case 'P':
        /* Zaustavlja se animacija. Odnosno pauza u igrici. */
        animation_ongoing = 0;
				pause = 1;
        break;

		/*Resetovanje igre koja nije zavrsena na pocetak.*/
		case 'r':
    case 'R':
				pogodak=0;
				animation_ongoing = 0;
				TIMER_INTERVAL = 50;
				x_vektora=0.25;
				y_vektora=0.25;
				glutPostRedisplay();
				break;

				/*Ukoliko je igra zavrsena moze se zapoceti nova.*/
				case 'n':
				case 'N':
				if(game_over == 1){
						pogodak=0;
						TIMER_INTERVAL = 50;
						game_over = 0;
						x_koordinata=(int)((float)rand()/RAND_MAX*16)-8;
						y_koordinata=(int)((float)rand()/RAND_MAX*17)-8.5;
						x_plocice=(int)((float)rand()/RAND_MAX*16)-8;
						x_vektora=0.25;
						y_vektora=0.25;
						glutPostRedisplay();
					}
						break;
    }
}

static void on_reshape(int width, int height)
{
   /* Podesava se viewport. */
    glViewport(0, 0, width, height);
    /* Podesava se projekcija. */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(30, (float) width / height, 1, 150);
}

/*Funkcija koja random bira broj iz niza koji cemo koristiti za koordinatu vektora
kako bi se povremeno menjalo kretanje loptice*/
float random_vector(void){
	float niz[] = {0.25,0.5,0.75,1};
	srand(time(NULL));
	int i = rand() % 4 + 1;
	return niz[i-1];
}

static void on_timer(int value)
{
    /* Proverava se da li callback dolazi od odgovarajuceg tajmera. */
    if (value != TIMER_ID)
        return;
		/*Pomeranje x koordinate loptice vektorim brzine.*/
    x_koordinata+=x_vektora;
    /*kolizija sa levim i desnim zidom*/
    if(x_koordinata <= -IVICA+1 || x_koordinata >= IVICA-1 ){
    		x_vektora *= -1;
			}
		/*Pomeranje y koordinate loptice vektorim brzine.*/
    y_koordinata += y_vektora;

    /*kolizija sa gornjim zidom*/
    if(y_koordinata>=IVICA-1)
        y_vektora*= -1;

    /* ---kolizija sa plocicom--- */
		/*racunamo euklidkso rastojanje izmedju centra lopte i centra plocice*/
    float rastojanje=sqrt((x_koordinata-x_plocice)*(x_koordinata-x_plocice)
											+ (y_koordinata-y_plocice)*(y_koordinata-y_plocice));

    /*ako je loptica na plocici-odredjeno po y koordinati centra loptice*/
    if(y_koordinata<=-8.5 && y_koordinata > -9.5){
			/*ako je blize sredini plocice-odbija se u drugom pravcu*/
        if(rastojanje>=MIN_RASTOJANJE && rastojanje<=1.5){
             y_vektora *= -1;
						 /*uvecavamo broj pogodaka (udaraca loptice o plocicu)*/
						 pogodak++;
						 /*-nakon svaka 2 pogotka loptica ce ubrzati svoje kretanje
						   - ubrzavanje traje do nekog trenutka jer timer ne sme dostici 0
							 kako se animacija ne bi zaustavila
							 -takodje posle odredjenog broja pogodaka menjamo kretanje loptice
							 tako sto promenimo koordinate vektora*/
						 if(pogodak % 2 == 0){
							 if(TIMER_INTERVAL>5)
							 			TIMER_INTERVAL-=5;
							 x_vektora = 0.25;
							 y_vektora = random_vector();
						 }
					 }
			/*ako je blize krajevima plocice-odbija se u istom pravcu,a suprotnom smeru*/
				if(rastojanje>1.5 && rastojanje<=MAX_RASTOJANJE) {
						 y_vektora *= -1;
		         x_vektora *= -1;
						 pogodak++;
						 /*-nakon svaka 2 pogotka loptica ce ubrzati svoje kretanje
						   - ubrzavanje traje do nekog trenutka jer timer ne sme dostici 0
							 kako se animacija ne bi zaustavila
							 -takodje posle odredjenog broja pogodaka menjamo kretanje loptice*/
						 if(pogodak % 2 == 0){
							 if (TIMER_INTERVAL >5)
										TIMER_INTERVAL-=5;
								x_vektora = random_vector();
 							 	y_vektora = 0.25;
							}
					 }
				 }

		 /*Ukoliko ne uhvatimo lopticu plocicom igra se prekida. Menjamo indikator
		 game_over na 1 kako bi se pozvala adekvatna funkcija za ispis/*/
    	 if(y_koordinata < -10.5 && rastojanje > MAX_RASTOJANJE ){
				 	animation_ongoing = 0;
					game_over = 1;
			 }

    /* Forsira se ponovno iscrtavanje prozora. */
    glutPostRedisplay();
    /* Po potrebi se ponovo postavlja tajmer. Ako animacija jos uvek nije prekinuta*/
    /*kao neki rekurzivni poziv funkcije*/
    if (animation_ongoing) {
        glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
    }
}
/*Funkcija za ispis poruke kada je igra zavrsena.*/
void ispisGameOver(void){
	glDisable(GL_LIGHTING);

	/*niske u kojima se cuva potreban tekst*/
	char tekst1[MAX_KARAKTERA], *p1;
	/*upisivanje teksta u promenljivu*/
	sprintf(tekst1, "Game over! :(\n");

	char tekst2[MAX_KARAKTERA], *p2;
	sprintf(tekst2, "Score:");

	char brUhvacenih[MAX_KARAKTERA], *p3;
	sprintf(brUhvacenih, "%d", pogodak);

	char opcija1[MAX_KARAKTERA], *p4;
	sprintf(opcija1, "Press 'n' for new game.");

	char opcija2[MAX_KARAKTERA], *p5;
	sprintf(opcija2, "Press 'esc' for exit.");

	/*pisanje jednog po jednog teksta pomocu funkcije glutBitmapCharacter*/
	glPushMatrix();
			glColor3f(0.8, 0, 0);
			glRasterPos3f(-3, 1, 0);
			for(p1 = tekst1; *p1!= '\0'; p1++){
					glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *p1);
			}
	glPopMatrix();

	glPushMatrix();
			glColor3f(0.8,0,0);
			glRasterPos3f(-3, 0, 0);
			for(p2 = tekst2; *p2!= '\0'; p2++){
					glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *p2);
			}
	glPopMatrix();

	glPushMatrix();
			glColor3f(0.8,0,0);
			glRasterPos3f(0, 0, 0);
			for(p3 = brUhvacenih; *p3!= '\0'; p3++){
					glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *p3);
			}
	glPopMatrix();

	glPushMatrix();
			glColor3f(1.0, 0.5, 0.0);
			glRasterPos3f(-4, -5, 0);
			for(p4 = opcija1; *p4!= '\0'; p4++){
					glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *p4);
			}
	glPopMatrix();

	glPushMatrix();
			glColor3f(0, 0.5, 0.5);
			glRasterPos3f(-4, -6, 0);
			for(p5 = opcija2; *p5!= '\0'; p5++){
					glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *p5);
			}
	glPopMatrix();

}

/*Funkcija za ispis trenutnog skora tokom trajanja igre.*/
void ispisTrenutniScore(void){
	glDisable(GL_LIGHTING);

	/*niske u kojima se cuva potreban tekst*/
	char tekst1[MAX_KARAKTERA], *p1;
	/*upisivanje teksta u promenljivu*/
	sprintf(tekst1, "Current score: ");

	char tekst2[MAX_KARAKTERA], *p2;
	sprintf(tekst2, "%d", pogodak);

	/*pisanje jednog po jednog teksta pomocu funkcije glutBitmapCharacter*/
	glPushMatrix();
			glColor3f(0,0,0);
			glRasterPos3f(-3, IVICA-0.55, 1.5);
			for(p1 = tekst1; *p1!= '\0'; p1++){
					glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *p1);
			}
	glPopMatrix();

	glPushMatrix();
			glColor3f(0,0,0);
			glRasterPos3f(2.5, IVICA-0.6, 1.5);
			for(p2 = tekst2; *p2!= '\0'; p2++){
					glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *p2);
			}
	glPopMatrix();
}

/*Funkcija za ispis poruke "Pause!" kada korisnik pritisne 'p'. */
void ispisPause(void){
	glDisable(GL_LIGHTING);

	/*niske u kojima se cuva potreban tekst*/
	char tekst1[MAX_KARAKTERA], *p1;
	/*upisivanje teksta u promenljivu*/
	sprintf(tekst1, "Pause!\n");

	glPushMatrix();
			glColor3f(1,1,0);
			glRasterPos3f(-1, 0, 0);
			for(p1 = tekst1; *p1!= '\0'; p1++){
					glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *p1);
			}
	glPopMatrix();

}

/*Funkcija koja crta objekte na sceni i podesava njihove materijale.*/
void nacrtaj_scenu(void){

	/*Koeficijenti materijala za lopticu: */
	/* Nulti koeficijenti refleksije materijala. */
	GLfloat no_material[] = { 0, 0, 0, 1 };
	/* Koeficijenti difuzne refleksije materijala. */
	GLfloat material_diffuse[] = { 0.1, 0.5, 0.8, 1 };
	/* Koeficijenti spekularne refleksije materijala. */
	GLfloat material_specular[] = { 1, 1, 1, 1 };
	/* Koeficijent spekularne refleksije za slucaj velike vrednosti koeficijenta. */
	GLfloat high_shininess[] = { 100 };

	/*Koeficijenti za zidove i plocicu: */
	GLfloat mat_ambient[] ={0.25f, 0.25f, 0.25f, 1.0f  };
	GLfloat mat_diffuse[] ={0.4f, 0.4f, 0.4f, 1.0f };
	GLfloat mat_specular[] ={0.774597f, 0.774597f, 0.774597f, 1.0f };
	GLfloat shine = 76.8f;

	/*levi zid*/
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_specular);
	glMaterialf(GL_FRONT, GL_SHININESS, shine);
	glTranslatef(-IVICA,0,0);
	glScalef(1,20,1);
	glutSolidCube(1);
	glPopMatrix();

	/*desni zid*/
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_specular);
	glMaterialf(GL_FRONT, GL_SHININESS, shine);
	glTranslatef(IVICA,0,0);
	glScalef(1,20,1);
	glutSolidCube(1);
	glPopMatrix();

	/*gornji zid*/
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_specular);
	glMaterialf(GL_FRONT, GL_SHININESS, shine);
	glTranslatef(0,IVICA,0);
	glScalef(21,1,1);
	glutSolidCube(1);
	glPopMatrix();

	/*plocica*/
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_specular);
	glMaterialf(GL_FRONT, GL_SHININESS, shine);
	glTranslatef(x_plocice,0,0);
	glTranslatef(0,y_plocice,0);
	glScalef(4,1,1);
	glutSolidCube(1);
	glPopMatrix();

	/*loptica*/
	glPushMatrix();
	glTranslatef(x_koordinata,y_koordinata,0);
	glMaterialfv(GL_FRONT, GL_AMBIENT, no_material);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);
	glMaterialfv(GL_FRONT, GL_EMISSION, no_material);
	glutSolidSphere(0.5,30,30);
	glPopMatrix();

	/*Crtanje poligona odredjene tekture koji ce biti pozadina na sceni.*/
	glBindTexture(GL_TEXTURE_2D, names[0]);
	glBegin(GL_QUADS);
			glNormal3f(1, 0, 0);

			glTexCoord2f(0, 0);
			glVertex3f(-20, -20, -5);

			glTexCoord2f(0, 1);
			glVertex3f(20, -20, -5);

			glTexCoord2f(1, 1);
			glVertex3f(20, 20, -5);

			glTexCoord2f(1, 0);
			glVertex3f(-20, 20, -5);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);
}

static void on_display(void)
{
    /* Postavlja se boja svih piksela na zadatu boju pozadine. */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/* Pozicija svetla (u pitanju je direkcionalno svetlo). */
		GLfloat light_position[] = { 7, 7, 3, 0 };
		/* Ambijentalna boja svetla. */
    GLfloat light_ambient[] = { 0, 0, 0, 1 };
    /* Difuzna boja svetla. */
    GLfloat light_diffuse[] = { 1, 1, 1, 1 };
    /* Spekularna boja svetla. */
    GLfloat light_specular[] = { 1, 1, 1, 1 };
    /* Ambijentalno osvetljenje scene. */
    GLfloat model_ambient[] = { 0.4, 0.4, 0.4, 1 };

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		/*Podesavanje osvetljenja.*/
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
		glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, model_ambient);

		/* Pozicionira se svijetlo. */
		glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 0, 40, 0, 0, 0, 0, 1, 0);

		/*Iscrtavamo objekte na sceni.*/
		nacrtaj_scenu();

		/*Dok traje igra ispisuje se trenutni skor.*/
		ispisTrenutniScore();

		/*Ukoliko je korisnik pritisnuo 'p' ispsuje se odgovarajuci tekst.*/
		if(pause == 1)
			ispisPause();

		/*Ukoliko je igra zavrsena ispisuje se tekst za kraj igre.*/
		if (game_over == 1)
			ispisGameOver();

    /* Nova slika se salje na ekran. */
    glutSwapBuffers();
}
