#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>



#define PROGRAM   1
#define ID        2
#define PV        3
#define PT        4
#define VAR       5
#define DP        6
#define INTEGER   7
#define CHAR_     8
#define BEGIN_    9
#define END_      10
#define VIRG      11
#define AFF       12
#define IF        13
#define THEN      14
#define ELSE      15
#define READ      16
#define READLN    17
#define WRITE     18
#define WRITELN   19
#define PO        20
#define PF        21
#define DO        22
#define WHILE     23
#define OPADD     24
#define OPREL     25
#define OPMUL     26
#define NB        27
#define FINPRG    100
#define ERROR     900



char *motcles[] =
{
    "program","var","integer","char","begin","end",
    "if","then","else","while","do",
    "read","readln","write","writeln"
};

int codemc[] =
{
    PROGRAM,VAR,INTEGER,CHAR_,BEGIN_,END_,
    IF,THEN,ELSE,WHILE,DO,
    READ,READLN,WRITE,WRITELN
};

#define NBCLE 15



char tab_id[200][30];
int nb_id = 0;



typedef struct {
    int ul;     
    int att;    
} unilex;



FILE *fp;
char lexeme[50];
char car;



void reculer() {
    fseek(fp, -1, SEEK_CUR);
}

char lire_car() {
    return fgetc(fp);
}

int est_mot_cle(char *s) {
    for (int i = 0; i < NBCLE; i++)
        if (strcmp(s, motcles[i]) == 0)
            return codemc[i];
    return 0;
}

int ranger_id(char *s) {
    for (int i = 0; i < nb_id; i++)
        if (strcmp(tab_id[i], s) == 0)
            return i;

    strcpy(tab_id[nb_id], s);
    nb_id++;
    return nb_id - 1;
}

char *get_symbole(unilex u) {
    static char buf[20];
    switch (u.ul) {
        case ID: return "id";
        case NB: return "nb";
        case OPADD: return "opadd";
        case OPREL: return "oprel";
        case OPMUL: return "opmul";
        case PV: return ";";
        case VIRG: return ",";
        case PO: return "(";
        case PF: return ")";
        case PT: return ".";
        case DP: return ":";
        case PROGRAM: return "program";
        case VAR: return "var";
        case INTEGER: return "integer";
        case CHAR_: return "char";
        case BEGIN_: return "begin";
        case END_: return "end";
        case IF: return "if";
        case THEN: return "then";
        case ELSE: return "else";
        case WHILE: return "while";
        case DO: return "do";
        case READ: return "read";
        case READLN: return "readln";
        case WRITE: return "write";
        case WRITELN: return "writeln";
        default:
            sprintf(buf, "unknown_%d", u.ul);
            return buf;
    }
}



unilex analex()
{
    unilex u;
    int i = 0;
    int etat = 0;

    while (1)
    {
        switch (etat)
        {
            case 0:
                car = lire_car();

                if (car == EOF)
                {
                    u.ul = FINPRG;
                    return u;
                }

                if (car==' ' || car=='\t') { etat = 0; break; }
                if (car=='\n') { etat = 0; break; }

                if (isalpha(car)) {
                    lexeme[0] = car;
                    i = 1;
                    etat = 1;
                    break;
                }

                if (isdigit(car)) {
                    lexeme[0] = car;
                    i = 1;
                    etat = 3;
                    break;
                }

                if (car=='<') etat = 5;
                else if (car=='>') etat = 8;
                else if (car=='=') { u.ul=OPREL; u.att='='; return u; }
                else if (car=='+'){ u.ul=OPADD; u.att='+'; return u; }
                else if (car=='-'){ u.ul=OPADD; u.att='-'; return u; }
                else if (car=='*'){ u.ul=OPMUL; u.att='*'; return u; }
                else if (car=='/'){ u.ul=OPMUL; u.att='/'; return u; }
                else if (car=='%'){ u.ul=OPMUL; u.att='%'; return u; }
                else if (car==';'){ u.ul=PV; return u; }
                else if (car==','){ u.ul=VIRG; return u; }
                else if (car=='('){ u.ul=PO; return u; }
                else if (car==')'){ u.ul=PF; return u; }
                else if (car=='.'){ u.ul=PT; return u; }
                else if (car==':'){ u.ul=DP; return u; }

                else {
                    printf("\nErreur lexicale : caractere inconnu '%c'\n",car);
                    exit(1);
                }
                break;

           
            case 1:
                car = lire_car();
                if (isalnum(car)) {
                    lexeme[i++] = car;
                } else {
                    lexeme[i] = '\0';
                    reculer();

                    int code = est_mot_cle(lexeme);
                    if (code != 0) {
                        u.ul = code;
                        return u;
                    } else {
                        u.ul = ID;
                        u.att = ranger_id(lexeme);
                        return u;
                    }
                }
                break;

            
            case 3:
                car = lire_car();
                if (isdigit(car)) {
                    lexeme[i++] = car;
                } else {
                    lexeme[i] = '\0';
                    reculer();
                    u.ul = NB;
                    u.att = atoi(lexeme);
                    return u;
                }
                break;

            
            case 5:
                car = lire_car();
                if (car=='='){ u.ul=OPREL; u.att=1; return u; }
                if (car=='>'){ u.ul=OPREL; u.att=2; return u; }
                reculer();
                u.ul = OPREL;
                u.att = 3; // <
                return u;

            
            case 8:
                car = lire_car();
                if (car=='='){ u.ul=OPREL; u.att=4; return u; }
                reculer();
                u.ul = OPREL;
                u.att = 5; // >
                return u;
        }
    }
}

int main() {
    char fichier[50];
    
    scanf("%s", fichier);

    fp = fopen(fichier, "r");
    if (!fp) { printf("Erreur fichier.\n"); return 1; }

    unilex u;
    do {
        u = analex();
        if (u.ul == FINPRG) break;
        printf("%s\n", get_symbole(u));
    } while (1);

    printf("\n--- Table des identificateurs ---\n");
    for (int i=0;i<nb_id;i++)
        printf("%d : %s\n", i, tab_id[i]);

    fclose(fp);
    return 0;
}