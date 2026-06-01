#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>


#define PROGRAM   1
#define ID        2
#define PV        3
#define PT        4
#define VAR       5
#define DP        6
#define INTEGER   7
#define CHAR_     8
#define BEGIN_    9
#define END_     10
#define VIRG     11
#define AFF      12
#define IF       13
#define THEN     14
#define ELSE     15
#define READ     16
#define READLN   17
#define WRITE    18
#define WRITELN  19
#define PO       20
#define PF       21
#define DO       22
#define WHILE    23
#define OPADD    24
#define OPREL    25
#define OPMUL    26
#define NB       27
#define STRING   28
#define FINPRG   100
#define ERROR    900


#define TYPE_INT 1
#define TYPE_CHAR 2
#define TYPE_ERROR 0


char *motcles[] = {
    "program","var","integer","char","begin","end",
    "if","then","else","while","do",
    "read","readln","write","writeln"
};
int codemc[] = {
    PROGRAM,VAR,INTEGER,CHAR_,BEGIN_,END_,
    IF,THEN,ELSE,WHILE,DO,
    READ,READLN,WRITE,WRITELN
};
#define NBCLE 15

const char *nom_unite[] = {
    "", "PROGRAM", "ID","PV","PT","VAR","DP","INTEGER","CHAR_",
    "BEGIN_","END_","VIRG","AFF","IF","THEN","ELSE","READ",
    "READLN","WRITE","WRITELN","PO","PF","DO","WHILE",
    "OPADD","OPREL","OPMUL","NB","STRING"
};


char tab_id[200][30]; int nb_id=0;
typedef struct { char nom[30]; int type; } Symbole;
Symbole table_symb[1000]; int nb_symboles=0;

typedef struct{int ul;int att;} unilex;

FILE *fp; char lexeme[256]; int car; unilex unite_courante;
int etiquette_count = 0;


int chercher_symb(char *nom){
    for(int i=0;i<nb_symboles;i++)
        if(!strcmp(table_symb[i].nom,nom)) return i;
    return -1;
}

void reculer(){ if(car!=EOF) fseek(fp,-1,SEEK_CUR); }
int lire_car(){ return fgetc(fp); }
int est_mot_cle(char *s){ for(int i=0;i<NBCLE;i++) if(!strcmp(s,motcles[i])) return codemc[i]; return 0; }

int ranger_id(char *s){
    for(int i=0;i<nb_id;i++) if(!strcmp(tab_id[i],s)) return i;
    strcpy(tab_id[nb_id],s); return nb_id++;
}

// ==================== ANALYSEUR LEXICAL ====================
unilex analex(){
    unilex u={0,0}; int i=0,etat=0;

    while(1){
        switch(etat){

        case 0:
            car=lire_car();
            if(car==EOF){ u.ul=FINPRG; return u; }
            if(isspace(car)){ if(car=='\n'){} break; }

            
            if(car=='{'){
                while((car=lire_car())!='}' && car!=EOF);
                break;
            }

            
            if(car=='('){
                int next = lire_car();
                if(next=='*'){
                    int prev = 0;
                    while(1){
                        car = lire_car();
                        if(car==EOF){ printf("Erreur: commentaire (* ... *) non termine\n"); exit(1); }
                        if(prev=='*' && car==')') break;
                        prev = car;
                    }
                    break;
                } else {
                    
                    reculer();
                }
            }

            if(car=='\''){ i=0;
                while((car=lire_car())!='\'' && car!=EOF && i < (int)sizeof(lexeme)-1) lexeme[i++]=car;
                lexeme[i]='\0';
                u.ul=STRING; return u;
            }

            if(isalpha(car)){ lexeme[0]=car; i=1; etat=1; break; }
            if(isdigit(car)){ lexeme[0]=car; i=1; etat=3; break;}

            switch(car){
                case ';': u.ul=PV; return u;
                case ',': u.ul=VIRG; return u;
                case '(': u.ul=PO; return u;
                case ')': u.ul=PF; return u;
                case '.': u.ul=PT; return u;
                case '+': u.ul=OPADD; u.att='+'; return u;
                case '-': u.ul=OPADD; u.att='-'; return u;
                case '*': u.ul=OPMUL; u.att='*'; return u;
                case '/': u.ul=OPMUL; u.att='/'; return u;
                case '%': u.ul=OPMUL; u.att='%'; return u;
                case '=': u.ul=OPREL; u.att='='; return u;
                case '|': { 
                    int n = lire_car();
                    if(n=='|'){ u.ul=OPADD; u.att='O'; return u; }
                    else { printf("Erreur lexicale: '|' isolé\n"); exit(1); }
                }
                case '&': { 
                    int n = lire_car();
                    if(n=='&'){ u.ul=OPMUL; u.att='A'; return u; }
                    else { printf("Erreur lexicale: '&' isolé\n"); exit(1); }
                }
            }

            if(car==':'){etat=10; break;}
            if(car=='<'){etat=5; break;}
            if(car=='>'){etat=8; break;}

            printf("Erreur lexicale '%c'\n",car); exit(1);


        case 1:
            car=lire_car();
            while(isalnum(car) && i < (int)sizeof(lexeme)-1){ lexeme[i++]=car; car=lire_car(); }
            lexeme[i]='\0';
            if(car!=EOF) reculer();
            int code=est_mot_cle(lexeme);
            if(code){ u.ul=code; return u;}
            u.ul=ID; u.att=ranger_id(lexeme); return u;

        case 3:
            car=lire_car();
            while(isdigit(car) && i < (int)sizeof(lexeme)-1){ lexeme[i++]=car; car=lire_car(); }
            lexeme[i]='\0';
            if(car!=EOF) reculer();
            u.ul=NB; u.att=atoi(lexeme); return u;

        case 5:
            car=lire_car();
            if(car=='='){ u.ul=OPREL; u.att='L'; return u; } 
            if(car=='>'){ u.ul=OPREL; u.att='N'; return u; } 
            reculer(); u.ul=OPREL; u.att='<'; return u;

        case 8:
            car=lire_car();
            if(car=='='){ u.ul=OPREL; u.att='G'; return u; } 
            reculer(); u.ul=OPREL; u.att='>'; return u;

        case 10:
            car=lire_car();
            if(car=='='){ u.ul=AFF; return u; }
            reculer(); u.ul=DP; return u;
        }
    }
}


// ========== CODE INTERMEDIAIRE MACHINE A PILE ============
char code_map[5000][120];
int nb_code=0;

void emettre(const char *s){
    if(nb_code<5000) strcpy(code_map[nb_code++], s);
    else { printf("Buffer code_map plein\n"); exit(1); }
}

void emettre_fmt(const char *fmt, ...){
    char buf[200];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    emettre(buf);
}

void afficher_code(){
    printf("\n\n========== CODE INTERMEDIAIRE (Machine a Pile) ==========\n");
    for(int i=0; i<nb_code; i++){
        printf("%4d : %s\n", i, code_map[i]);
    }
    printf("=========================================================\n");
}

int nouvelle_etiquette(){
    return etiquette_count++;
}

// ==================== SYNTAXE + GENERATION MAP + SEMANTIQUE ====================
void accepter(int t){
    if(unite_courante.ul!=t){
        printf("Erreur syntaxique attendu %s trouve %s\n", nom_unite[t],
               (unite_courante.ul < 29? nom_unite[unite_courante.ul] : "INCONNU"));
        exit(1);
    }
    unite_courante = analex();
}


int Facteur();
int Terme();
int Exp_simple();
int Exp();
void Condition();
void Instruction();
void BlocInstructions();

// Facteur -> nb | id | 'str' | ( Exp_simple )
int Facteur(){
    char tmp[200];
    int tp = TYPE_ERROR;

    if(unite_courante.ul==NB){
        sprintf(tmp,"Empiler %d", unite_courante.att);
        emettre(tmp);
        tp = TYPE_INT;
        accepter(NB);
        return tp;
    }
    else if(unite_courante.ul==ID){
        
        char *name = tab_id[unite_courante.att];
        int pos = chercher_symb(name);
        if(pos == -1){
            printf("Erreur semantique: variable '%s' non declaree\n", name);
            exit(1);
        }
        sprintf(tmp,"Valeurd @%s", tab_id[unite_courante.att]);
        emettre(tmp);
        tp = table_symb[pos].type;
        accepter(ID);
        return tp;
    }
    else if(unite_courante.ul==STRING){
        
        sprintf(tmp,"Empiler '%s'", lexeme);
        emettre(tmp);
        tp = TYPE_CHAR; 
        accepter(STRING);
        return tp;
    }
    else if(unite_courante.ul==PO){
        accepter(PO);
        tp = Exp_simple();
        accepter(PF);
        return tp;
    }
    else{
        printf("Erreur Facteur: token %d\n", unite_courante.ul); exit(1);
    }
}

// Terme -> Facteur { (*|/|%|&&) Facteur }

int Terme(){
    int type_left = Facteur();
    while(unite_courante.ul==OPMUL){
        int op = unite_courante.att;
        accepter(OPMUL);
        int type_right = Facteur();
    
        if(op == '*' || op == '/' || op == '%' ){
            if(type_left != TYPE_INT || type_right != TYPE_INT){
                printf("Erreur semantique: operation arithmetique require integer * / %% (types %d et %d)\n", type_left, type_right);
                exit(1);
            }
            type_left = TYPE_INT;
        } else if(op == 'A'){ 
            if(type_left != TYPE_INT || type_right != TYPE_INT){
                printf("Erreur semantique: '&&' require integer (as boolean) operands\n");
                exit(1);
            }
            
            type_left = TYPE_INT;
        }
        
        if(op=='*') emettre("*");
        else if(op=='/') emettre("/");
        else if(op=='%') emettre("%");
        else if(op=='A') emettre("&&");
    }
    return type_left;
}

// Exp_simple -> Terme { (+|-|||) Terme }
int Exp_simple(){
    int type_left = Terme();
    while(unite_courante.ul==OPADD){
        int op = unite_courante.att;
        accepter(OPADD);
        int type_right = Terme();
        
        if(op=='+' || op=='-'){
            if(type_left != TYPE_INT || type_right != TYPE_INT){
                printf("Erreur semantique: + et - require integer operands\n");
                exit(1);
            }
            type_left = TYPE_INT;
        } else if(op=='O'){ 
            if(type_left != TYPE_INT || type_right != TYPE_INT){
                printf("Erreur semantique: '||' require integer operands\n");
                exit(1);
            }
            type_left = TYPE_INT;
        }
        
        if(op=='+') emettre("+");
        else if(op=='-') emettre("-");
        else if(op=='O') emettre("||");
    }
    return type_left;
}

// Exp -> Exp_simple | Exp_simple oprel Exp_simple

int Exp(){
    int t1 = Exp_simple();
    if(unite_courante.ul==OPREL){
        int op = unite_courante.att;
        accepter(OPREL);
        int t2 = Exp_simple();
        
        if(t1==TYPE_ERROR || t2==TYPE_ERROR){
            printf("Erreur semantique: expression invalide\n"); exit(1);
        }
        if(t1 != t2){
            printf("Erreur semantique: comparaison entre types differents (%d vs %d)\n", t1, t2);
            exit(1);
        }
        
        switch(op){
            case '<': emettre("Comparer-si-inf"); break;
            case 'L': emettre("Comparer-si-infe"); break; 
            case '>': emettre("Comparer-si-sup"); break;
            case 'G': emettre("Comparer-si-supe"); break; 
            case '=': emettre("Comparer-si-egal"); break;
            case 'N': emettre("Comparer-si-diff"); break; 
            default: emettre("Comparer-si-egal"); break;
        }
        return TYPE_INT; 
    }
    return t1;
}

// BlocInstructions -> BEGIN ... END  ou instruction simple
void BlocInstructions(){
    if(unite_courante.ul==BEGIN_){
        accepter(BEGIN_);
        while(unite_courante.ul!=END_){
            Instruction();
        }
        accepter(END_);
    } else {
        Instruction();
    }
}

// Instruction
void Instruction(){
    char tmp[200];

    // Affectation ID := Exp_simple ;
    if(unite_courante.ul==ID){
        char var[60];
        strcpy(var, tab_id[unite_courante.att]);
        int pos = chercher_symb(var);
        if(pos==-1){
            printf("Erreur semantique: variable '%s' non declaree (affectation)\n", var);
            exit(1);
        }
        accepter(ID);
        accepter(AFF);

        
        sprintf(tmp,"Valeurg @%s", var);
        emettre(tmp);

        int t_expr = Exp_simple();

        
        int type_var = table_symb[pos].type;
        if(type_var != t_expr){
            printf("Erreur semantique: affectation type mismatch pour %s (variable type %d, expr type %d)\n", var, type_var, t_expr);
            exit(1);
        }

        emettre(":=");
        accepter(PV);
        return;
    }

    // IF cond THEN ... [ELSE ...]
    if(unite_courante.ul==IF){
        accepter(IF);
        
        int tcond = Exp();
        if(tcond != TYPE_INT){
            printf("Erreur semantique: condition IF doit etre de type boolean (integer resultat)\n");
            exit(1);
        }

        int e_sinon = nouvelle_etiquette();
        int e_fin = nouvelle_etiquette();

        emettre_fmt("Aller-si-faux E%d", e_sinon);
        accepter(THEN);

        BlocInstructions();

        if(unite_courante.ul==ELSE){
            emettre_fmt("Aller-a E%d", e_fin);
            emettre_fmt("Etiq E%d", e_sinon);
            accepter(ELSE);
            BlocInstructions();
            emettre_fmt("Etiq E%d", e_fin);
        } else {
            emettre_fmt("Etiq E%d", e_sinon);
        }

        if(unite_courante.ul==PV) accepter(PV);
        return;
    }

    // WHILE cond DO ...
    if(unite_courante.ul==WHILE){
        int e_deb = nouvelle_etiquette();
        int e_fin = nouvelle_etiquette();

        emettre_fmt("Etiq E%d", e_deb);
        accepter(WHILE);

        int tcond = Exp();
        if(tcond != TYPE_INT){
            printf("Erreur semantique: condition WHILE doit etre de type boolean (integer resultat)\n");
            exit(1);
        }

        emettre_fmt("Aller-si-faux E%d", e_fin);

        accepter(DO);
        BlocInstructions();

        emettre_fmt("Aller-a E%d", e_deb);
        emettre_fmt("Etiq E%d", e_fin);

        if(unite_courante.ul==PV) accepter(PV);
        return;
    }

    // READ / READLN
    if(unite_courante.ul==READ || unite_courante.ul==READLN){
        int is_readln = (unite_courante.ul==READLN);
        int token = unite_courante.ul;
        accepter(unite_courante.ul);
        accepter(PO);
        if(unite_courante.ul==ID){
            char *name = tab_id[unite_courante.att];
            int pos = chercher_symb(name);
            if(pos==-1){ printf("Erreur semantique: variable '%s' non declaree pour read\n", name); exit(1); }
            sprintf(tmp,"Readln @%s", tab_id[unite_courante.att]);
            emettre(tmp);
            accepter(ID);
        } else {
            printf("Read attendu ID\n"); exit(1);
        }
        accepter(PF);
        accepter(PV);
        return;
    }

    // WRITE / WRITELN
    if(unite_courante.ul==WRITE || unite_courante.ul==WRITELN){
        int is_writeln = (unite_courante.ul==WRITELN);
        accepter(unite_courante.ul);
        accepter(PO);

        // liste d'arguments (id | nb | 'str') séparés par virgule
        if(unite_courante.ul!=PF){
            while(1){
                if(unite_courante.ul==STRING){
                    sprintf(tmp,"Empiler '%s'", lexeme);
                    emettre(tmp);
                    accepter(STRING);
                } else if(unite_courante.ul==ID){
                    char *name = tab_id[unite_courante.att];
                    int pos = chercher_symb(name);
                    if(pos==-1){ printf("Erreur semantique: variable '%s' non declaree pour write\n", name); exit(1); }
                    sprintf(tmp,"Valeurd @%s", tab_id[unite_courante.att]);
                    emettre(tmp);
                    accepter(ID);
                } else if(unite_courante.ul==NB){
                    sprintf(tmp,"Empiler %d", unite_courante.att);
                    emettre(tmp);
                    accepter(NB);
                } else {
                    printf("Write: argument inattendu\n"); exit(1);
                }

                if(unite_courante.ul==VIRG){
                    accepter(VIRG);
                    continue;
                } else break;
            }
        }

        accepter(PF);
        accepter(PV);

        if(is_writeln) emettre("Writeln");
        else emettre("Write");
        return;
    }

    // BEGIN ... END bloc déjà géré par BlocInstructions mais ....
    if(unite_courante.ul==BEGIN_){
        accepter(BEGIN_);
        while(unite_courante.ul!=END_){
            Instruction();
        }
        accepter(END_);
        if(unite_courante.ul==PV) accepter(PV);
        return;
    }

    // instruction vide
    if(unite_courante.ul==PV){
        accepter(PV);
        return;
    }

    
    printf("Erreur Instruction: token %d non gere\n", unite_courante.ul);
    exit(1);
}


// Programme
void Programme(){
    accepter(PROGRAM); accepter(ID); accepter(PV);

    while(unite_courante.ul==VAR){
        accepter(VAR);
        char liste[50][30]; int n=0;

        do{
            if(unite_courante.ul!=ID) exit(1);
            strcpy(liste[n++], tab_id[unite_courante.att]);
            accepter(ID);
            if(unite_courante.ul==VIRG) accepter(VIRG);
            else if(unite_courante.ul==DP){ accepter(DP); break; }
        } while(1);

        int type = (unite_courante.ul==INTEGER)?TYPE_INT:TYPE_CHAR;
        if(unite_courante.ul==INTEGER) accepter(INTEGER);
        else if(unite_courante.ul==CHAR_) accepter(CHAR_);
        else exit(1);
        accepter(PV);

        for(int i=0;i<n;i++){
            if(chercher_symb(liste[i])!=-1) {
                printf("Erreur: variable deja declaree %s\n", liste[i]); exit(1);
            }
            strcpy(table_symb[nb_symboles].nom, liste[i]);
            table_symb[nb_symboles].type = type;
            nb_symboles++;
        }
    }

    accepter(BEGIN_);
    while(unite_courante.ul!=END_) Instruction();
    accepter(END_); accepter(PT);

    emettre("Halt");
}
typedef struct {
    char mot[20];
    char tokenName[20];
} MotCleAff;

MotCleAff table_affichage_mots_cles[] = {
    {"program", "PROGRAM"},
    {"var", "VAR"},
    {"integer", "INTEGER"},
    {"char", "CHAR_"},
    {"begin", "BEGIN_"},
    {"end", "END_"},
    {"if", "IF"},
    {"then", "THEN"},
    {"else", "ELSE"},
    {"while", "WHILE"},
    {"do", "DO"},
    {"read", "READ"},
    {"readln", "READLN"},
    {"write", "WRITE"},
    {"writeln", "WRITELN"}
};
int nb_aff_mc = 15;


void afficher_table_mots_cles() {
    printf("\n=================================\n");
    printf("      TABLE DES MOTS-CLES\n");
    printf("=================================\n");
    printf(" %-10s | %s\n", "Mot-cle", "Token");
    printf("---------------------------------\n");

    for(int i=0; i<nb_aff_mc; i++){
        printf(" %-10s | %s\n", table_affichage_mots_cles[i].mot, table_affichage_mots_cles[i].tokenName);
    }

    printf("=================================\n");
}


// ==================== MAIN ====================
int main(){
    char fichier[256];
    printf("Nom fichier source : ");
    scanf("%s",fichier);
    fp = fopen(fichier,"r");
    if(!fp){ printf("Fichier introuvable\n"); return 0; }

    unite_courante = analex();
    Programme();

    printf("\n============ COMPILATION REUSSIE ============\n");

    printf("\n--- Table des Identificateurs (lexeme order) ---\n");
    for(int i=0;i<nb_id;i++) printf("  %2d : %s\n", i, tab_id[i]);

    printf("\n--- Table des Symboles (declarations) ---\n");
    for(int i=0;i<nb_symboles;i++)
        printf("  %2d : %-15s Type: %s\n", i, table_symb[i].nom,
               table_symb[i].type==TYPE_INT?"INTEGER":"CHAR");
    afficher_table_mots_cles();
    afficher_code();

    fclose(fp);
    return 0;
}
