program test1;
var
  num, counter: integer;
  letter: char;
begin
  num := 10;
  counter := 0;
  read(letter);
  if num > 5 then
    while counter < num do
      begin
        counter := counter + 1;
        write(counter);
      end
  else
    writeln(letter);
end.