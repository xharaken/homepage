#include "common.h"
#include "common.c"

typedef struct word_t
{
  char begin;
  char end;
  char used;
  char str[17];
}word_t;

word_t word[1000];
int ind[30][1000];
int usedlist[1000];

int main(int argc, char **argv)
{
  FILE *fp;
  int i, j, u, t, times, word_num, cur, prev, start, count, point, point2, point3, ran, hit, hit_min;
  char line[20], pat[20];
  int num[30];
  
  if(argc != 2)
    {
      outn("usage : %s file", argv[0]);
      return 1;
    }
  
  for(i = 0; i < 30; i++)
    {
      num[i] = 0;
    }
  fp = fopen(argv[1], "rb");
  word_num = 0;
  while(!feof(fp))
    {
      fgets(line, 100, fp);
      line[strlen(line) - 1] = 0;
      word[word_num].begin = line[0] - 'a';
      word[word_num].end = line[strlen(line) - 1] - 'a';
      word[word_num].used = 0;
      strcpy(word[word_num].str, line);
      ind[word[word_num].begin][num[word[word_num].begin]++] = word_num;
      word_num++;
    }
  fclose(fp);
  
  while(1)
    {
      while(1)
        {
          out("enemy: ");
          scanf(" %s %d", pat, &times);
          for(i = 0; i < word_num; i++)
            {
              if(!strcmp(word[i].str, pat))
                {
                  break;
                }
            }
          if(i < word_num)
            {
              word[i].used = 1;
              break;
            }
        }
      
      start = word[i].end;
      for(u = 0; u < num[start]; u++)
        {
          if(word[ind[start][u]].used == 0)
            {
              word[ind[start][u]].used = 1;
              point = 0;
              for(t = 0; t < times; t++)
                {
                  cur = word[ind[start][u]].end;
                  count = 0;
                  while(1)
                    {
                      if(num[cur] == 0)
                        {
                          break;
                        }
                      if(count % 2)
                        {
                          ran = mrand_int(0, num[cur] - 1);
                          for(i = 0; i < num[cur]; i++)
                            {
                              if(word[ind[cur][ran]].used == 0)
                                {
                                  break;
                                }
                              ran = (ran + 1) % num[cur];
                            }
                          if(i == num[cur])
                            {
                              break;
                            }
                        }
                      else
                        {
                          hit_min = 1000;
                          for(i = 0; i < num[cur]; i++)
                            {
                              if(word[ind[cur][i]].used == 0)
                                {
                                  hit = 0;
                                  for(j = 0; j < num[word[ind[cur][i]].end]; j++)
                                    {
                                      if(word[ind[word[ind[cur][i]].end][j]].used == 0)
                                        {
                                          hit++;
                                        }
                                    }
                                  if(hit < hit_min)
                                    {
                                      hit_min = hit;
                                      ran = i;
                                    }
                                }
                            }
                          if(hit_min == 1000)
                            {
                              break;
                            }
                        }
                      usedlist[count++] = ind[cur][ran];
                      word[ind[cur][ran]].used = 1;
                      cur = word[ind[cur][ran]].end;
                    }
                  point += (1 - count % 2);
                  for(i = 0; i < count; i++)
                    {
                      word[usedlist[i]].used = 0;
                    }
                }
              word[ind[start][u]].used = 0;

              word[ind[start][u]].used = 1;
              point2 = 0;
              for(t = 0; t < times; t++)
                {
                  cur = word[ind[start][u]].end;
                  prev = word[ind[start][u]].begin;
                  count = 0;
                  while(1)
                    {
                      if(num[cur] == 0)
                        {
                          break;
                        }
                      if(count % 2)
                        {
                          ran = mrand_int(0, num[cur] - 1);
                          for(i = 0; i < num[cur]; i++)
                            {
                              if(word[ind[cur][ran]].used == 0)
                                {
                                  break;
                                }
                              ran = (ran + 1) % num[cur];
                            }
                          if(i == num[cur])
                            {
                              break;
                            }
                        }
                      else
                        {
                          hit_min = 1000;
                          for(i = 0; i < num[cur]; i++)
                            {
                              if(word[ind[cur][i]].used == 0)
                                {
                                  if(word[ind[cur][i]].end == prev)
                                    {
                                      hit_min = 0;
                                      ran = i;
                                      break;
                                    }
                                  hit = 0;
                                  for(j = 0; j < num[word[ind[cur][i]].end]; j++)
                                    {
                                      if(word[ind[word[ind[cur][i]].end][j]].used == 0)
                                        {
                                          hit++;
                                        }
                                    }
                                  if(hit < hit_min)
                                    {
                                      hit_min = hit;
                                      ran = i;
                                    }
                                }
                            }
                          if(hit_min == 1000)
                            {
                              break;
                            }
                        }
                      usedlist[count++] = ind[cur][ran];
                      word[ind[cur][ran]].used = 1;
                      cur = word[ind[cur][ran]].end;
                      //prev = word[ind[cur][ran]].begin;
                    }
                  point2 += (1 - count % 2);
                  for(i = 0; i < count; i++)
                    {
                      word[usedlist[i]].used = 0;
                    }
                }
              word[ind[start][u]].used = 0;
              
              word[ind[start][u]].used = 1;
              point3 = 0;
              for(t = 0; t < times; t++)
                {
                  cur = word[ind[start][u]].end;
                  prev = word[ind[start][u]].begin;
                  count = 0;
                  while(1)
                    {
                      if(num[cur] == 0)
                        {
                          break;
                        }
                      if(count % 2)
                        {
                          ran = mrand_int(0, num[cur] - 1);
                          for(i = 0; i < num[cur]; i++)
                            {
                              if(word[ind[cur][ran]].used == 0)
                                {
                                  break;
                                }
                              ran = (ran + 1) % num[cur];
                            }
                          if(i == num[cur])
                            {
                              break;
                            }
                        }
                      else
                        {
                          hit_min = 1000;
                          for(i = 0; i < num[cur]; i++)
                            {
                              if(word[ind[cur][i]].used == 0)
                                {
                                  if(word[ind[cur][i]].end == prev)
                                    {
                                      hit_min = 0;
                                      ran = i;
                                      break;
                                    }
                                  hit = 0;
                                  for(j = 0; j < num[word[ind[cur][i]].end]; j++)
                                    {
                                      if(word[ind[word[ind[cur][i]].end][j]].used == 0)
                                        {
                                          hit++;
                                        }
                                    }
                                  if(hit < hit_min)
                                    {
                                      hit_min = hit;
                                      ran = i;
                                    }
                                }
                            }
                          if(hit_min == 1000)
                            {
                              break;
                            }
                        }
                      usedlist[count++] = ind[cur][ran];
                      word[ind[cur][ran]].used = 1;
                      cur = word[ind[cur][ran]].end;
                      if(count == 1)
                        {
                          prev = word[ind[cur][ran]].begin;
                        }
                    }
                  point3 += (1 - count % 2);
                  for(i = 0; i < count; i++)
                    {
                      word[usedlist[i]].used = 0;
                    }
                }
              word[ind[start][u]].used = 0;
              outn("%s : %d %d %d", word[ind[start][u]].str, point, point2, point3);
            }
        }
      
      while(1)
        {
          out("you: ");
          scanf(" %s", pat);
          for(i = 0; i < word_num; i++)
            {
              if(!strcmp(word[i].str, pat))
                {
                  break;
                }
            }
          if(i < word_num)
            {
              word[i].used = 1;
              break;
            }
        }
    }
  return 0;
}
