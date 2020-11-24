#include <stdio.h>
#include <stdlib.h>

char **read_img(int *n, int **v) //functie care returneaza un pointer la matricea imaginii si citeste n si numarul de coloane aferente fiecarei linii
{
    char **img = NULL;
    int i, j;
    unsigned int temp;
    scanf("%d", n);
    *v = (int *)malloc(*n * sizeof(int) * 4);
    img = (char **)malloc(*n * sizeof(char*));
    for(i = 0; i < *n; i++)
    {
        scanf("%d", *v + i);
        img[i] = (char *)malloc(*(*v + i) * sizeof(char) * 4);
        for(j = 0; j < *(*v + i); j++)
        {
            scanf("%X", &temp);
            //printf("%02X %02X %02X %02X\n", (temp&0xFF000000)>>24, (temp&0x00FF0000)>>16, (temp&0x0000FF00)>>8, temp &0x000000FF);
            img[i][j * 4 + 3] = (temp & 0xFF000000)>>24;
            img[i][j * 4 + 2] = (temp & 0x00FF0000)>>16;
            img[i][j * 4 + 1] = (temp & 0x0000FF00)>>8;
            img[i][j * 4] = temp & 0x000000FF;
        }
        *(*v + i) *= 4;
    }
    return img;
}

void modify(char **img, int *nr_col, int size, int line, int col, int val)
{
    if(col + 4 > nr_col[line])
    {
        img[line] = (char *)realloc(img[line], (col + 4) * sizeof(char));
        nr_col[line] = col + 4;
    }
    if(size == 1){
        img[line][col] = (char)val;
    }
    else if(size == 2)
    {
        img[line][col] = (char)(val & 0x000000FF);
        img[line][col + 1] = (char)((val & 0x0000FF00)>>8);
    }
    else if(size == 4)
    {
        img[line][col + 3] = (char)((val & 0xFF000000)>>24);
        img[line][col + 2] = (char)((val & 0x00FF0000)>>16);
        img[line][col + 1] = (char)((val & 0x0000FF00)>>8);
        img[line][col] = (char)(val & 0x000000FF);
    }
}

void delete(char **img, int size, int line, int col)
{
    for(int i = 0; i < size; i++)
        img[line][col + i] = 0;
}

void swap(char **img, int size, int line, int col)
{
    char aux;
    if(size == 2)
    {
        aux = img[line][col];
        img[line][col] = img[line][col + 1];
        img[line][col + 1] = aux;
    }
    if(size == 4)
    {
        aux = img[line][col + 3];
        img[line][col + 3] = img[line][col];
        img[line][col] = aux;
        aux = img[line][col + 2];
        img[line][col + 2] = img[line][col + 1];
        img[line][col + 1] = aux;
    }
}

void modify_img(char **img, int *nr_col)
{
    int k;
    char cmd, size;
    int line, dist, val;
    scanf("%d", &k);
    for(int i = 0; i < k; i++)
    {
        scanf("\n%c %c %d %d", &cmd, &size, &line, &dist);
        if(cmd == 'M')
            scanf("%X", &val);
        if(size == 'C')
            size = 1;
        if(size == 'S')
            size = 2;
        if(size == 'I')
            size = 4;
        dist--;//datele de intrare dau blocurile incepand cu 1 iar coloanele mele incep de la 0
        dist *= (int)size;
        if(cmd == 'M')
            modify(img, nr_col, (int)size, line, dist, val);
        if(cmd == 'D')
            delete(img, (int)size, line, dist);
        if(cmd == 'S')
            swap(img, size, line, dist + size);
    }
}

double get_shield_power(int n, int *nr_col, char **img)
{
    int i, j, count_bytes = 0;
    long long sum_bytes = 0;
    for (i = 1; i < n - 1; i++)
    {
        count_bytes += 2;
        sum_bytes += (int)img[i][0] + (int)img[i][*(nr_col + i) - 1];
    }
    for (j = 0; j < *nr_col; j++)
    {
        count_bytes++;
        sum_bytes += (int)img[0][j];
    }
    for (j = 0; j < *(nr_col + n - 1); j++)
    {
        count_bytes++;
        sum_bytes += (int)img[n - 1][j];
    }
    return (double)sum_bytes/count_bytes;
}

int black_hole_finder(char **img, int *nr_col, int line, int col)
{
    char move_x[] = {1, 0, 0, -1};
    char move_y[] = {0, 1, -1, 0};
    for(int i = 0; i < 4; i++)
        if(line + (int)move_x[i] >= 0 && col + (int)move_y[i] < *(nr_col + line + (int)move_x[i]))
            if(img[line + (int)move_x[i]][col + (int)move_y[i]] == 0)
                return 1 + black_hole_finder(img, nr_col, line + (int)move_x[i], col + (int)move_y[i]);
    return 0;
}

int main()
{
    char **img;
    int n, *nr_col;
    img = read_img(&n, &nr_col);
    printf("task 1\n%.8lf\n", get_shield_power(n, nr_col, img));
    modify_img(img, nr_col);
    printf("task 2\n");
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < *(nr_col + i); j+= 4)
        {
            for(int k = 3; k >= 0; k--)
                printf("%02X", (int)img[i][j + k]&0x000000FF);
            printf(" ");
        }
        printf("\n");
    }
    int xmax = 0, ymax = 0, super_hole_size = 0, max_size = 0;
    /*for(int i = 0; i < n; i++)
    {
        for(int j = 0; j < *(nr_col + i); j++)
            if(img[i][j] == 0)
            {
                super_hole_size = black_hole_finder(img, nr_col, i, j);
                if(super_hole_size > max_size)
                {
                    max_size = super_hole_size;
                    xmax = i;
                    ymax = j;
                }
            }
    }*/
    max_size = black_hole_finder(img, nr_col, 9, 8);
    printf("task 3\n%d %d %d\n", xmax, ymax, max_size);
    for(int i = 0; i < n; i++)
        free(*(img + i));
    free(img);
    free(nr_col);
    return 0;
}