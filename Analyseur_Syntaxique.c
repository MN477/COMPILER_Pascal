#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char symbole[50];

void P(); 
void DCL();
void DCL_1();
void Liste_Id();
void Liste_Id_1(); 
void type();
void Inst_Composee();
void Inst();
void Liste_Inst();
void Liste_Inst_1();
void I();
void Exp();
void Exp_1();
void Exp_Simple();
void Exp_Simple_1();
void Terme();
void Terme_1();
void Facteur();

char symbole[50];
int pos_erreur =0;
char dernier_sym_correct[50]="";
char procedure_courrante[50]="";


void proc_en_cours(const char *nom_proc)
{
    strcpy(procedure_courrante , nom_proc);
}

void suivant()
{ if(strcmp(symbole, "fin prg") == 0)
        return;
  printf("donner le symbole suivant:\n");
  scanf("%s", symbole);
  pos_erreur = pos_erreur + 1;
}

 void erreur(const char *message , const char *conseil)
{
    printf("Erreur syntaxique a la position %d de votre sequence du prg saisie : dans la regle '%s' du symbole courant '%s': %s\n" , pos_erreur ,procedure_courrante,symbole, message);
    
    if(conseil != NULL)
    {
        printf("%s\n", conseil);
        
    }
    exit(1);

}


void accepter(const char *attendu)
{
    if(strcmp(symbole, attendu) == 0)
    { 
      strcpy(dernier_sym_correct, symbole);
      if(strcmp(symbole, ".") == 0) 
            strcpy(symbole, "fin prg");
      else
        suivant();
    }
    else
    {
        char message[400];
        char conseil [400];
        sprintf(message, "Erreur syntaxique: '%s' attendu, mais trouve '%s' apres '%s'", attendu, symbole,dernier_sym_correct);

        if (strcmp(attendu, "then") == 0)
            strcpy(conseil, "Vous avez oublie 'then' apres la condition 'if'.");
        else if (strcmp(attendu, "do") == 0)
            strcpy(conseil, "Vous avez oublie 'do' apres la condition 'while'.");
        else if (strcmp(attendu, ";") == 0)
            strcpy(conseil, "Vous avez oublie un ';' entre deux instructions.");
        else if (strcmp(attendu, "begin") == 0)
            strcpy(conseil, "Vous avez oublie 'begin' avant le bloc d'instructions.");
        else
            strcpy(conseil, "Verifiez le symbole attendu.");  
        erreur(message , conseil );
    }
}



void P()
{
    proc_en_cours("P");
    accepter("program");
    accepter("id");
    accepter(";");
    DCL();
    Inst_Composee();
    accepter(".");
    printf("Analyse Syntaxique reussie \n");
}

void DCL()
{
    proc_en_cours("DCL");
    DCL_1();
}

void DCL_1()
{   
    proc_en_cours("DCL_1");
    if(strcmp(symbole, "var")==0)
    {
        accepter("var");
        Liste_Id();
        accepter(":");

        type();
        accepter(";");
        DCL_1();
    }
}

void Liste_Id()
{   
    proc_en_cours("Liste_Id");
    accepter("id");
    Liste_Id_1();
}

void Liste_Id_1()
{   
    proc_en_cours("Liste_Id_1");
    if(strcmp(symbole , ",")==0)
    {
        accepter(",");
        accepter("id");
        Liste_Id_1();
    }
}

void type()
{   
    proc_en_cours("type");
    if(strcmp(symbole , "integer")==0)
    {
        accepter("integer");
    }
    else if (strcmp(symbole,"char")==0)
    {
        accepter("char");
    }
    else
    {
         erreur("Type attendu (integer or char)", "Le type doit etre soit 'integer' soit 'char'.");
    }
}

void Inst_Composee()
{   
    proc_en_cours("Inst_Composee");
    accepter("begin");
    Inst();
    accepter("end");
}

void Inst()
{   
    proc_en_cours("Inst");
    Liste_Inst();
}

void Liste_Inst()
{   
    proc_en_cours("Liste_Inst");
    I();
    Liste_Inst_1();
}

void Liste_Inst_1()
{   
    proc_en_cours("Liste_Inst_1");
    if(strcmp(symbole, ";")==0)
    {
        accepter(";");
        I();
        Liste_Inst_1();
    }
}

void I()
{   
    proc_en_cours("I");
    if(strcmp(symbole , "id")==0)
    {
        accepter("id");
        accepter(":");
        accepter("=");
        Exp_Simple();
    }
    else if (strcmp(symbole, "if")==0)
    {
        accepter("if");
        Exp();
        accepter("then");
        I();
        accepter("else");
        I();
    }
    else if (strcmp(symbole , "while")==0)
    {
        accepter("while");
        Exp();
        accepter("do");
        I();
    }
    else if ((strcmp(symbole , "read")==0)||(strcmp(symbole,"readln")==0)||
             (strcmp(symbole , "write")==0)||(strcmp(symbole , "writeln")==0))
    {
        accepter(symbole);
        accepter("(");
        accepter("id");
        accepter(")");
    }
    else
    {
       
        erreur("Instruction invalide", "Verifiez la syntaxe de l'instruction (id, if, while, read/write).");

    }
}

void Exp()
{   
    proc_en_cours("Exp");
    Exp_Simple();
    Exp_1();
}

void Exp_1()
{   
    proc_en_cours("Exp_1");
    if(strcmp(symbole , "oprel")==0)
    {
        accepter("oprel");
        Exp_Simple();
    }
}

void Exp_Simple()
{   
    proc_en_cours("Exp_Simple");
    Terme();
    Exp_Simple_1();
}

void Exp_Simple_1()
{   
    proc_en_cours("Exp_Simple_1");
    if(strcmp(symbole , "opadd")==0)
    {
        accepter("opadd");
        Terme();
        Exp_Simple_1();
    }
}

void Terme()
{   
    proc_en_cours("Terme");
    Facteur();
    Terme_1();
}

void Terme_1()
{   
    proc_en_cours("Terme_1");
    if(strcmp(symbole,"opmul")==0)
    {
        accepter("opmul");
        Facteur();
        Terme_1();
    }
}

void Facteur()
{   
    proc_en_cours("Facteur");
    if(strcmp(symbole , "id")==0)
    {
        accepter("id");
    }
    else if (strcmp(symbole , "nb")==0)
    {
        accepter("nb");
    }
    else if (strcmp(symbole , "(")==0)
    {
        accepter("(");
        Exp_Simple();
        accepter(")");
    }
    else
    {
        
       erreur("Facteur invalide", "Un facteur doit être un identifiant, un nombre ou une expression entre parenthèses.");

    }
}

int main()
{
    printf("entrez votre serie de symboles du prg: \n");
    suivant();
    P();
    return 0;
}
