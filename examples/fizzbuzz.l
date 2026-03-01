{ ismod0 (n n -- b) % 0 = }
{ fizz? (n -- b) 3 ismod0 [ "fizz" print true ] [ false ] ifelse }
{ buzz? (n -- b) 5 ismod0 [ "buzz" print true ] [ false ] ifelse }

{ fizzbuzz (n -- ) dup dup fizz? swap buzz? or not [ print ] if }

1 100 [ dup fizzbuzz ] for
