#include "common.h"
#include "common.c"

int dist, dist_min, n, sp;
int used[100], adj[100][100], stack[100];
char place[100][100];

void search(int cur, int count)
{
  int i;
  
  //outn("%d %d %d", cur, count, dist);
  if(count == n - 1)
    {
      dist += adj[cur][0];
      stack[sp++] = 0;
      if(dist <= dist_min)
        {
          dist_min = dist;
          for(i = 0; i < sp; i++)
            {
              out("%s ", place[stack[i]]);
            }
          outn("%d", dist_min);
        }
      --sp;
      dist -= adj[cur][0];
      return;
    }
  
  for(i = 0; i < n; i++)
    {
      if(used[i] == 0)
        {
          used[i] = 1;
          dist += adj[cur][i];
          stack[sp++] = i;
          search(i, count + 1);
          --sp;
          dist -= adj[cur][i];
          used[i] = 0;
        }
    }
  return;
}

int main(int argc, char **argv)
{
  int i, j, dist;
  char str1[100], str2[100], str[256];
  FILE *fp;
  
  if(argc != 3)
    {
      outn("usage : %s file n", argv[0]);
      return 1;
    }
  n = atoi(argv[2]);
  fp = fopen(argv[1], "rb");
  if(fp == NULL) error();
  
  for(i = 0; i < n; i++)
    {
      used[i] = 0;
      place[i][0] = 0;
    }
      
  for(i = 0; i < n; i++)
    {
      for(j = 0; j < n; j++)
        {
          fgets(str, 256, fp);          
          sscanf(str, "%s %s %d", str1, str2, &dist);
          if(place[i][0] && strcmp(str1, place[i])) error();
          strcpy(place[i], str1);
          if(place[j][0] && strcmp(str2, place[j])) error();
          strcpy(place[j], str2);
          adj[i][j] = dist;
          out(" %d", dist);
        }
      outn("");
    }
  fclose(fp);
  
  dist = 0;
  dist_min = 1000000000;
  used[0] = 1;
  sp = 0;
  stack[sp++] = 0;
  search(0, 0);
  
  outn("%d", dist_min);
  return 0;
}
