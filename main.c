#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

typedef struct
{
    char cuv_rom[30];
    char cuv_limba_str[30];
}Cuvant;

typedef struct
{
    int cuv_trad;
    int cuv_part_trad;
    int cuv_intrad;
}meta_dict;

int cmp_lungime(const void *a,const void *b)
{
    Cuvant *A = (Cuvant*)a;
    Cuvant *B = (Cuvant*)b;

    return strlen(A->cuv_rom) - strlen(B->cuv_rom);
}

Cuvant* incarca_dictionar(char nume_fisier[],int *n)
{
    int i;
    char cuv_ro[30],cuv_ls[30];
    FILE *f = fopen(nume_fisier,"r");
    fscanf(f,"%d",n);
    Cuvant *dict = (Cuvant*) malloc(sizeof(Cuvant) * *n);

    for(i=0;i<*n;i++)
    {
        fscanf(f,"%s%s",cuv_ro,cuv_ls);
        strcpy(dict[i].cuv_rom,cuv_ro);
        strcpy(dict[i].cuv_limba_str,cuv_ls);
    }

    qsort(dict,*n,sizeof(Cuvant),cmp_lungime);

    free(dict);
    fclose(f);
    return dict;
}

void afis_dictionar(Cuvant *dict, int n)
{
    int i;
    for(i=0;i<n;i++)
        printf("%s %s\n",dict[i].cuv_rom,dict[i].cuv_limba_str);
}

char* traducere(char cuvant[],Cuvant *dict,int n)
{
    int i;
    for(i=0;i<n;++i)
        if(strcmp(cuvant,dict[i].cuv_rom)==0)
            return dict[i].cuv_limba_str;

    return 0;
}

char* get_text(char nume_fisier[])
{
    FILE *f = fopen(nume_fisier,"r");
    int size;
    fseek(f,0,SEEK_END);
    size = ftell(f);

    fseek(f,0,SEEK_SET);
    char *text = (char*) malloc(sizeof(char)*size);
    char line[1000];
    fgets(text,1000,f);
    while(fgets(line,1000,f) != NULL)
        strcat(text,line);

    return text;
    fclose(f);
}

meta_dict calc_text(char text[],Cuvant *dict,int n)
{
    meta_dict meta;
    meta.cuv_intrad = 0;
    meta.cuv_part_trad = 0;
    meta.cuv_trad = 0;
    char sep[] = " \n,;'?!";
   // text = get_text("text_romana.txt");
    char *cuv = strtok(text,sep);
    while(cuv)
    {
        if(traducere(cuv,dict,n) != 0) meta.cuv_trad++;
        else
        {
            cuv[strlen(cuv) - 1] = 0;
            if(traducere(cuv,dict,n) != 0) meta.cuv_part_trad++;
            else
            {
                cuv[strlen(cuv) - 1] = 0;
                if(traducere(cuv,dict,n) != 0) meta.cuv_part_trad++;
                else meta.cuv_intrad++;
            }
        }
        cuv = strtok(NULL, sep);
    }

    return meta;
}

float indice_trad(char text[],Cuvant *dict,int n)
{
    meta_dict meta = calc_text(text,dict,n);
    return ((float)(meta.cuv_part_trad + meta.cuv_trad) / (meta.cuv_intrad + meta.cuv_part_trad + meta.cuv_trad));
}

char* det_lb_straina(char nume_fisier[],int n, ...)
{
    int i;
    char *text = get_text("text_romana.txt");
    char nume_fisier_str[30],fis_str[30];
    char ctext[1000];
    va_list fisiere_str;
    va_start(fisiere_str,n);
    float max_trad = 0,trad;

    for(i=0;i<n;i++)
    {
        int nr;
        strcpy(fis_str,va_arg(fisiere_str,char*));
        strcpy(ctext,text);
        Cuvant *dict = incarca_dictionar(fis_str,&nr);

        trad = indice_trad(ctext,dict,nr);
        if(trad > max_trad)
        {
            max_trad = trad;
            strcpy(nume_fisier_str,fis_str);
        }
    }

    free(text);
    return nume_fisier_str;
}

void traducere_text(char text[],char nume_dict[])
{
    int nr;
    Cuvant *dict = incarca_dictionar(nume_dict,&nr);

    char sep[] = " \n,;'?!";
    char *cuv = strtok(text,sep);
    while(cuv)
    {
        char cv[30];
        strcpy(cv,cuv);
        char *tr_cuv = traducere(cv,dict,nr);
        if(tr_cuv != 0) printf("%s ",tr_cuv);
        else
        {
            cv[strlen(cv) - 1] = 0;
            tr_cuv = traducere(cv,dict,nr);
            if(tr_cuv != 0) printf("%s? ",tr_cuv);
            else
            {
                cv[strlen(cv) - 1] = 0;
                tr_cuv = traducere(cv,dict,nr);
                if(tr_cuv != 0) printf("%s?? ",tr_cuv);
                else
                {
                    printf("[%s] ",cuv);
                }
            }
        }
        cuv = strtok(NULL, sep);
    }

    free(dict);
}

int main()
{
    int i,nr_dict_de,nr_dict_en,nr_dict_fr;
    Cuvant *dict_en = incarca_dictionar("dictionar_engleza.txt",&nr_dict_en);
    Cuvant *dict_fr = incarca_dictionar("dictionar_franceza.txt",&nr_dict_fr);
    Cuvant *dict_de = incarca_dictionar("dictionar_german.txt",&nr_dict_de);
    //afis_dictionar(dict_en,nr_dict_en);

    //printf("%s",traducere("pizda",dict_en,nr_dict_en));
    //meta_dict meta_en = calc_text(get_text("text_romana.txt"),dict_en,nr_dict_en);
    //printf("%f",indice_trad(get_text("text_romana.txt"),dict_de,nr_dict_de));
    char *nume_fis = det_lb_straina("text_romana.txt",3,"dictionar_german.txt","dictionar_engleza.txt","dictionar_franceza.txt");
    char cp[30];
    strcpy(cp,nume_fis);
    ///printf("%s",cp);

    traducere_text(get_text("text_romana.txt"),cp);

    free(dict_de);
    free(dict_en);
    free(dict_fr);
}
