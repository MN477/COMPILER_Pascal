#include <stdio.h> // printf , scanf , fopen , fgetc 
#include <stdlib.h>// exit atoi , malloc 
#include <string.h>// stcmp , strcpy , strlen 
#include <ctype.h>//isalpha , isdigit , ispace 
#include <stdarg.h> // gerer les fcts avec parametre varialbe emmtre_fmt

// ==================== CODES DES UNITÉS LEXICALES  chaque token recoit un numero pour simplifier ====================
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

// Type codes for semantic checking
#define TYPE_INT 1
#define TYPE_CHAR 2
#define TYPE_ERROR 0

// ==================== MOTS-CLÉS ====================
char *motcles[] = { // Liste de tous les mots réservés du langage. chaque elt est un pointeur vers un char plus simple pour strcmp
    "program","var","integer","char","begin","end",
    "if","then","else","while","do",
    "read","readln","write","writeln"
};
int codemc[] = { //  code token correspondant pour chaque mot-clé.
    PROGRAM,VAR,INTEGER,CHAR_,BEGIN_,END_,
    IF,THEN,ELSE,WHILE,DO,
    READ,READLN,WRITE,WRITELN
};
#define NBCLE 15

const char *nom_unite[] = { // Sert uniquement pour afficher les erreurs.attendu PROGRAM trouvé ID »
    "", "PROGRAM", "ID","PV","PT","VAR","DP","INTEGER","CHAR_",
    "BEGIN_","END_","VIRG","AFF","IF","THEN","ELSE","READ",
    "READLN","WRITE","WRITELN","PO","PF","DO","WHILE",
    "OPADD","OPREL","OPMUL","NB","STRING"
};

// tables
char tab_id[200][30]; int nb_id=0; // tableau qui stocke les noms des identifiants (variables) +combien d’identificateurs ont été trouvés
typedef struct { char nom[30]; int type; } Symbole;
Symbole table_symb[1000]; int nb_symboles=0;

typedef struct{int ul;int att;} unilex; // ul = code du token , att = attribut (id index, nombre, opérateur, etc.)

FILE *fp;// fichier source  
char lexeme[256]; // contenu textuel du mot lu (ex : identificateur, nombre)
int car;// dernier caractère lu
unilex unite_courante; // token en cours
int etiquette_count = 0;// → utilisé pour générer E0, E1, E2 dans le code intermédiaire MAP (pour les if, while…)

// ==================== TABLE & MEMO ====================
int chercher_symb(char *nom){// fonction qui cherche si nom (nom de variable) est dans la table des symboles ; retourne l’indice si trouvé, sinon -1.
    for(int i=0;i<nb_symboles;i++)
        if(!strcmp(table_symb[i].nom,nom)) return i;
    return -1;
}

void reculer(){ if(car!=EOF) fseek(fp,-1,SEEK_CUR); }//recule d’un caractère dans le fichier ouvert fp (remet le curseur un caractère en arrière).
int lire_car(){ return fgetc(fp); }//appelle fgetc sur le fichier fp et renvoie la valeur (le code du caractère ou EOF)
int est_mot_cle(char *s){ for(int i=0;i<NBCLE;i++) if(!strcmp(s,motcles[i])) return codemc[i]; return 0; }//prend une chaîne s (lexème lu) et vérifie si c’est un mot-clé.

int ranger_id(char *s){//ajoute l’identificateur s à la table lexicale tab_id si ce n’est pas déjà présent ; retourne son index.
    for(int i=0;i<nb_id;i++) if(!strcmp(tab_id[i],s)) return i;//parcourt les identifiants déjà stockés ; si s existe déjà, retourne son index i.
    strcpy(tab_id[nb_id],s);//copie la chaîne s dans la case libre tab_id[nb_id] (ajout d’un nouvel identificateur).
    return nb_id++;
}

// ==================== ANALYSEUR LEXICAL ====================
unilex analex(){//fonction qui renvoie un token (unilex contient ul = code du token, att = attribut).
    unilex u={0,0}; //initialise la structure de retour (ul=0, att=0).
    int i=0,//i est index pour remplir le buffer lexeme
    etat=0;//etat est la variable d’état d’un petit automate (0 = état initial).

    while(1){
        switch(etat){

        case 0:
            car=lire_car();//lit un caractère du fichier
            if(car==EOF){ u.ul=FINPRG; return u; }
            if(isspace(car)){ if(car=='\n'){} break; }//lit un caractère du fichier

            // commentaires { ... } (optionnel si présent)
            if(car=='{'){
                while((car=lire_car())!='}' && car!=EOF);
                break;
            }

            // commentaires (* ... *)
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
                    // pas un commentaire, remettre le caractere lu
                    reculer();
                }
            }

            if(car=='\''){ i=0; //Si on rencontre ', on lit tous les caractères suivants jusqu’à la prochaine ' ou EOF
                while((car=lire_car())!='\'' && car!=EOF && i < (int)sizeof(lexeme)-1) lexeme[i++]=car; //Les caractères sont copiés dans lexeme[] (avec protection i < sizeof(lexeme)-1 pour éviter overflow).
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
                case '|': { // expect ||
                    int n = lire_car();
                    if(n=='|'){ u.ul=OPADD; u.att='O'; return u; }
                    else { printf("Erreur lexicale: '|' isole\n"); exit(1); }
                }
                case '&': { // expect &&
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
            while(isalnum(car) && i < (int)sizeof(lexeme)-1){ lexeme[i++]=car; car=lire_car(); }//: tant que le caractère est alphanumérique, on l’ajoute au lexeme. Limite i pour éviter overflow.
            lexeme[i]='\0';
            if(car!=EOF) reculer();
            int code=est_mot_cle(lexeme);//on teste si lexeme est un mot-clé
            if(code){ u.ul=code; return u;}//code != 0 → c’est un mot-clé
            u.ul=ID; u.att=ranger_id(lexeme); return u;//Sinon c’est un identificateur  on enregistre/obtient l’index de l’identificateur dans tab_id.

        case 3:
            car=lire_car();
            while(isdigit(car) && i < (int)sizeof(lexeme)-1){ lexeme[i++]=car; car=lire_car(); }
            lexeme[i]='\0';
            if(car!=EOF) reculer();
            u.ul=NB; u.att=atoi(lexeme); return u;//valeur entière convertie

        case 5:
            car=lire_car();
            if(car=='='){ u.ul=OPREL; u.att='L'; return u; } // <= -> 'L'
            if(car=='>'){ u.ul=OPREL; u.att='N'; return u; } // <> -> 'N'
            reculer(); u.ul=OPREL; u.att='<'; return u;

        case 8:
            car=lire_car();
            if(car=='='){ u.ul=OPREL; u.att='G'; return u; } // >= -> 'G'
            reculer(); u.ul=OPREL; u.att='>'; return u;

        case 10:
            car=lire_car();
            if(car=='='){ u.ul=AFF; return u; }
            reculer(); u.ul=DP; return u;//:
        }
    }
}


// ========== CODE INTERMEDIAIRE MACHINE A PILE =============
char code_map[5000][120];//tableau de 5000 lignes, chaque ligne pouvant contenir jusqu’à 120 caractères. Chaque ligne représente une instruction de code intermédiaire (ex : "LOAD x", "ADD", "JMP 12"…)
int nb_code=0;//Nombre d’instructions déjà stockées.

void emettre(const char *s){//ajouter une instruction simple dans code_map.
    if(nb_code<5000) strcpy(code_map[nb_code++], s);
    else { printf("Buffer code_map plein\n"); exit(1); }
}

void emettre_fmt(const char *fmt, ...){//C’est une version formatée de emettre, comme printf
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

int nouvelle_etiquette(){//Génère une nouvelle étiquette (label) unique.
    return etiquette_count++;
}

// ==================== SYNTAXE + GENERATION MAP + SEMANTIQUE ====================
void accepter(int t){
    if(unite_courante.ul!=t){
        printf("Erreur syntaxique attendu %s trouve %s\n", nom_unite[t],
               (unite_courante.ul < 29? nom_unite[unite_courante.ul] : "INCONNU"));
        exit(1);
    }
    unite_courante = analex();//si tout va bien, lit le token suivant (avance le scanner)
}

// forward (now returning types)
int Facteur();//gère nb, id, string, (Exp_simple)
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
        // verifier declaration
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
        // si chaîne: empiler la chaîne littérale
        sprintf(tmp,"Empiler '%s'", lexeme);
        emettre(tmp);
        tp = TYPE_CHAR; // behandel strings as char type
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
// return type (for arithmetic: integer)
int Terme(){
    int type_left = Facteur();
    while(unite_courante.ul==OPMUL){
        int op = unite_courante.att;
        accepter(OPMUL);
        int type_right = Facteur();
        // semantic checks
        if(op == '*' || op == '/' || op == '%' ){
            if(type_left != TYPE_INT || type_right != TYPE_INT){
                printf("Erreur semantique: operation arithmetique require integer * / %% (types %d et %d)\n", type_left, type_right);
                exit(1);
            }
            type_left = TYPE_INT;
        } else if(op == 'A'){ // 'A' for &&
            if(type_left != TYPE_INT || type_right != TYPE_INT){
                printf("Erreur semantique: '&&' require integer (as boolean) operands\n");
                exit(1);
            }
            // logical op results in integer (0/1)
            type_left = TYPE_INT;
        }
        // generate code for op
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
        // semantic checks
        if(op=='+' || op=='-'){
            if(type_left != TYPE_INT || type_right != TYPE_INT){
                printf("Erreur semantique: + et - require integer operands\n");
                exit(1);
            }
            type_left = TYPE_INT;
        } else if(op=='O'){ // 'O' corresponds to ||
            if(type_left != TYPE_INT || type_right != TYPE_INT){
                printf("Erreur semantique: '||' require integer operands\n");
                exit(1);
            }
            type_left = TYPE_INT;
        }
        // generate code
        if(op=='+') emettre("+");
        else if(op=='-') emettre("-");
        else if(op=='O') emettre("||");
    }
    return type_left;
}

// Exp -> Exp_simple | Exp_simple oprel Exp_simple
// If oprel present result is integer (boolean)
int Exp(){
    int t1 = Exp_simple();
    if(unite_courante.ul==OPREL){
        int op = unite_courante.att;
        accepter(OPREL);
        int t2 = Exp_simple();
        // checks: require same type (or both integer/char) - choose same type
        if(t1==TYPE_ERROR || t2==TYPE_ERROR){
            printf("Erreur semantique: expression invalide\n"); exit(1);
        }
        if(t1 != t2){
            printf("Erreur semantique: comparaison entre types differents (%d vs %d)\n", t1, t2);
            exit(1);
        }
        // emit compare instruction according to op
        switch(op){
            case '<': emettre("Comparer-si-inf"); break;
            case 'L': emettre("Comparer-si-infe"); break; // <=
            case '>': emettre("Comparer-si-sup"); break;
            case 'G': emettre("Comparer-si-supe"); break; // >=
            case '=': emettre("Comparer-si-egal"); break;
            case 'N': emettre("Comparer-si-diff"); break; // <>
            default: emettre("Comparer-si-egal"); break;
        }
        return TYPE_INT; // boolean result
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
    char tmp[200];//buffer temporaire pour construire des chaînes d’instructions MAP (ex : "Empiler 3", "Valeurd @x").

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

        // generate address first
        sprintf(tmp,"Valeurg @%s", var);//empiler l’adresse de la variable)
        emettre(tmp);

        int t_expr = Exp_simple();

        // type check for assignment
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
        // Condition uses Exp (which emits compare)
        int tcond = Exp();
        if(tcond != TYPE_INT){
            printf("Erreur semantique: condition IF doit etre de type boolean (integer resultat)\n");
            exit(1);
        }

        int e_sinon = nouvelle_etiquette();//création de deux étiquettes uniques pour gérer les sauts.
        int e_fin = nouvelle_etiquette();

        emettre_fmt("Aller-si-faux E%d", e_sinon);//si condition fausse, sauter au bloc else (ou après le if).
        accepter(THEN);

        BlocInstructions();

        if(unite_courante.ul==ELSE){
            emettre_fmt("Aller-a E%d", e_fin);//(pour sauter le bloc else après le then)
            emettre_fmt("Etiq E%d", e_sinon);//(destination du saut si condition fausse)
            accepter(ELSE);//on consomme ELSE, génère bloc else, puis place Etiq e_fin.
            BlocInstructions();
            emettre_fmt("Etiq E%d", e_fin);
        } else {//(pas d’else) : on place Etiq e_sinon directement après le then.
            emettre_fmt("Etiq E%d", e_sinon);
        }

        if(unite_courante.ul==PV) accepter(PV);
        return;
    }

    // WHILE cond DO ...
    if(unite_courante.ul==WHILE){
        int e_deb = nouvelle_etiquette();// Création d’étiquettes pour début et fin de boucle.
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

        emettre_fmt("Aller-a E%d", e_deb);//À la fin du bloc on Aller-a Edeb pour répéter.
        emettre_fmt("Etiq E%d", e_fin);//Place Etiq Efin pour la sortie.

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

    // BEGIN ... END bloc déjà géré par BlocInstructions mais on supporte ici aussi
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

    // si rien reconnu — erreur
    printf("Erreur Instruction: token %d non gere\n", unite_courante.ul);
    exit(1);
}


// Programme
void Programme(){
    accepter(PROGRAM); accepter(ID); accepter(PV);

    while(unite_courante.ul==VAR){
        accepter(VAR);
        char liste[50][30]; int n=0;//tableau qui stocke tous les identificateurs déclarés dans la même lign


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

    emettre("Halt");//
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
