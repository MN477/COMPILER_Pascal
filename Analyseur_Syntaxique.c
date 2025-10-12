#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char symbole[50];

void P(); 
void DCL();
void DCL_Prime();
void Liste_Id();
void Liste_Id_Prime(); 
void type();
void Inst_Composee();
void Inst();
void Liste_Inst();
void Liste_Inst_Prime();
void I();
void Exp();
void Exp_Prime();
void Exp_Simple();
void Exp_Simple_Prime();
void Terme();
void Terme_Prime();
void Facteur();
void Facteur_Prime();

void erreur(const char *message)
{
    printf("%s\n", message);
    exit(1);
}

void suivant()
{ if(strcmp(symbole, "fin prg") == 0)
        return;
  printf("donner le symbole suivant:\n");
  scanf("%s", symbole);
}

void accepter(const char *attendu)
{
    if(strcmp(symbole, attendu) == 0)
    {
      if(strcmp(symbole, ".") == 0) 
            strcpy(symbole, "fin prg");
      else
        suivant();
    }
    else
    {
        char message[150];
        sprintf(message, "Erreur syntaxique: '%s' attendu, trouvé '%s'", attendu, symbole);
        erreur(message);
    }
}

void P()
{
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
    DCL_Prime();
}

void DCL_Prime()
{
    if(strcmp(symbole, "var")==0)
    {
        accepter("var");
        Liste_Id();
        accepter(":");

        type();
        accepter(";");
        DCL_Prime();
    }
}

void Liste_Id()
{
    accepter("id");
    Liste_Id_Prime();
}

void Liste_Id_Prime()
{
    if(strcmp(symbole , ",")==0)
    {
        accepter(",");
        accepter("id");
        Liste_Id_Prime();
    }
}

void type()
{
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
        erreur("Type attendu (integer or char)");
    }
}

void Inst_Composee()
{
    accepter("begin");
    Inst();
    accepter("end");
}

void Inst()
{
    Liste_Inst();
}

void Liste_Inst()
{
    I();
    Liste_Inst_Prime();
}

void Liste_Inst_Prime()
{
    if(strcmp(symbole, ";")==0)
    {
        accepter(";");
        I();
        Liste_Inst_Prime();
    }
}

void I()
{
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
        erreur("Instruction invalide");
    }
}

void Exp()
{
    Exp_Simple();
    Exp_Prime();
}

void Exp_Prime()
{
    if(strcmp(symbole , "oprel")==0)
    {
        accepter("oprel");
        Exp_Simple();
    }
}

void Exp_Simple()
{
    Terme();
    Exp_Simple_Prime();
}

void Exp_Simple_Prime()
{
    if(strcmp(symbole , "opadd")==0)
    {
        accepter("opadd");
        Terme();
        Exp_Simple_Prime();
    }
}

void Terme()
{
    Facteur();
    Terme_Prime();
}

void Terme_Prime()
{
    if(strcmp(symbole,"opmul")==0)
    {
        accepter("opmul");
        Facteur();
        Terme_Prime();
    }
}

void Facteur()
{
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
        erreur("Facteur invalide");
    }
}

int main()
{
    printf("entrez votre serie de symboles du prg: \n");
    suivant();
    P();
    return 0;
}
