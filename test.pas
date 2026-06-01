program test;

var age, salaire, choix, compteur, nombre, total : integer;
var    sexe : char;

begin
    
    age := 25;
    salaire := 3000 + 200 * 3;
    sexe := 'M';
    compteur := 0;
    choix := 1;
    total := 0;

    
    
    
    while choix <> 0 do
    begin
        writeln('');
        writeln('--- MENU PRINCIPAL ---');
        writeln('1. Test age et majorite');
        writeln('2. Calcul salaire');
        writeln('3. Compteur et boucles');
        writeln('4. Calculs arithmetiques');
        writeln('0. Quitter');
        write('Choix : ');
        readln(choix);
        
        { Option 1: Test d'age }
        if choix = 1 then
        begin
            write('Votre age : ');
            readln(age);
            
            if age >= 18 then
            begin
                writeln('>>> Vous etes MAJEUR');
                
                if age >= 65 then
                begin
                    writeln('>>> Retraite possible');
                end
                else
                begin
                    if age >= 50 then
                    begin
                        writeln('>>> Senior actif');
                    end
                    else
                    begin
                        writeln('>>> Adulte actif');
                    end;
                end;
            end
            else
            begin
                writeln('>>> Vous etes MINEUR');
                
                if age >= 16 then
                begin
                    writeln('>>> Adolescent');
                end
                else
                begin
                    writeln('>>> Enfant ou jeune ado');
                end;
            end;
        end;
        
        { Option 2: Calcul salaire }
        if choix = 2 then
        begin
            write('Salaire actuel : ');
            readln(salaire);
            
            writeln('Analyse du salaire...');
            
            if salaire < 2000 then
            begin
                writeln('Salaire faible');
                salaire := salaire + 500;
                writeln('Bonus ajoute');
                write('Nouveau salaire : ');
                write(salaire);
            end
            else
            begin
                if salaire < 5000 then
                begin
                    writeln('Salaire moyen');
                    salaire := salaire + 300;
                    write('Nouveau salaire : ');
                    write(salaire);
                end
                else
                begin
                    writeln('Bon salaire');
                    writeln('Pas de bonus necessaire');
                end;
            end;
        end;
        
        { Option 3: Compteur }
        if choix = 3 then
        begin
            write('Nombre iterations : ');
            readln(nombre);
            compteur := 0;
            total := 0;
            
            writeln('Debut du comptage...');
            
            while compteur < nombre do
            begin
                compteur := compteur + 1;
                total := total + compteur;
                write('Iteration : ');
                write(compteur);
            end;
            
            writeln('Comptage termine');
            write('Total accumule : ');
            write(total);
        end;
        
        { Option 4: Calculs }
        if choix = 4 then
        begin
            write('Premier nombre : ');
            readln(nombre);
            write('Second nombre : ');
            readln(compteur);
            
            total := nombre + compteur;
            write('Addition : ');
            write(total);
            
            total := nombre * compteur;
            write('Multiplication : ');
            write(total);
            
            if nombre > compteur then
            begin
                writeln('Premier > Second');
                total := nombre - compteur;
            end
            else
            begin
                writeln('Second >= Premier');
                total := compteur - nombre;
            end;
            
            write('Difference : ');
            write(total);
        end;
        
        
        if choix > 4 then
        begin
            writeln('ERREUR: Choix invalide');
        end;
    end;
    
    
end.