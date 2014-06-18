/*
 * Arvores binarias de busca
 */


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "abb.h"
#include "map.h"


int main() {
  ABB a;
  
  srand(time(NULL));

  for (int i = 0; i < 5; i++)
  {
    Pos p(rand() % 5, rand() % 5);
    printf("\ninsere: (%d %d)", p.getX(), p.getY());
    if (a.insere(p))
      printf(" OK");
    else
      printf(" ja tem");
  }

  printf("\n\n");

  Pos p(rand() % 5, rand() % 5);

  printf("\nbusca: (%d %d)", p.getX(), p.getY());

  if (a.busca(p))
    printf(" Encontrou");
  else
    printf(" Nao encontrou");

  printf("\nEm: ");
  a.inorder();

  printf("\n");

  for (int i = 0; i < 5; i++)
  {
    int x, y;

    printf("\n\n remove: ");
    scanf("%d %d", &x, &y);

    Pos rem(x, y);
    if(a.remove(rem))
      printf(" achou");

    else
      printf(" nao achou");

    printf("\nEm: ");
    a.inorder();
  }

  //libera(a);
  return 0;

}
    
