#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

typedef struct
{
    unsigned char R,G,B;
} pixel;

typedef struct
{
    unsigned latime, inaltime;

    pixel* pixeli;
} imagine;

imagine citire_imagine(char *fisier)
{
    FILE *f=fopen(fisier,"rb");

    if (f == NULL)
    {
        printf("eroare la deschiderea fisierului %s\n", fisier);
        exit(0);
    }

    imagine img;

    fseek(f,18,SEEK_SET);
    fread(&img.latime,sizeof(unsigned int),1,f);
    fread(&img.inaltime,sizeof(unsigned int),1,f);

    img.pixeli=(pixel*)malloc(img.latime*img.inaltime*sizeof(pixel));
    fseek(f,54,SEEK_SET);

    for(int i=0;i<img.latime*img.inaltime;i++)
    {
        pixel p;
        fread(&p.B,1,1,f);
        fread(&p.G,1,1,f);
        fread(&p.R,1,1,f);
        img.pixeli[i] = p;
    }

    fclose(f);

    return img;
}

void scriere_imagine(imagine img, char* original, char* fisier) {
    FILE *f=fopen(fisier,"wb");

    if (f == NULL)
    {
        printf("eroare la deschiderea fisierului %s\n", fisier);
        exit(0);
    }

    FILE *g=fopen(original,"rb");

    if (g == NULL)
    {
        printf("eroare la deschiderea fisierului %s\n", original);
        exit(0);
    }

    char* header = (char*)malloc(54);

    fread(header, 1, 54, g);
    fwrite(header, 1, 54, f);

    free(header);

    fclose(g);

    fseek(f,54,SEEK_SET);

    for(int i=0;i<img.latime*img.inaltime;i++)
    {
        pixel p=img.pixeli[i];
        fwrite(&p.B,1,1,f);
        fwrite(&p.G,1,1,f);
        fwrite(&p.R,1,1,f);
    }

    fclose(f);

}

unsigned int xorshift32(unsigned int state)
{
	state ^= state << 13;
	state ^= state >> 17;
	state ^= state << 5;
	return state;
}

unsigned int* generare_vector(unsigned int latime, unsigned int inaltime, unsigned int R0)
{
    unsigned n=2*latime*inaltime-1;
    unsigned *vect=(unsigned int*)malloc(n*sizeof(unsigned int));

    vect[0]=xorshift32(R0);

    for(int i=1;i<n;i++)
        vect[i]=xorshift32(vect[i-1]);

    return vect;
}

unsigned int* generare_permutare(imagine img, unsigned int R[])
{
    unsigned int n=img.latime*img.inaltime;
    unsigned int *permutare=(unsigned int*) malloc(n*sizeof(unsigned int));

    int i;
    for(i=0;i<n;i++)
        permutare[i]=i;

    for(i=n-1;i>=1;i--)
    {
        int j=R[n-i]%(i+1);
        unsigned int aux=permutare[i];
        permutare[i]=permutare[j];
        permutare[j]=aux;
    }

    return permutare;
}

imagine permuta_imagine(imagine img, unsigned int P[]) {
    imagine permutata;
    permutata.latime=img.latime;
    permutata.inaltime=img.inaltime;
    permutata.pixeli=(pixel*)malloc(img.latime*img.inaltime*sizeof(pixel));

    int i;
    for(i=0; i<img.latime*img.inaltime; ++i)
        permutata.pixeli[P[i]] = img.pixeli[i];

    return permutata;
}

void criptare(char* f_necriptata, char* f_criptata, char* f_secret_key) {
    FILE *g=fopen(f_secret_key,"r");

    if (g == NULL)
    {
        printf("eroare la deschiderea fisierului %s\n", f_secret_key);
        exit(0);
    }

    unsigned int R0;
    fscanf(g,"%u",&R0);

    unsigned int SV;
    fscanf(g,"%u",&SV);

    fclose(g);

    imagine img=citire_imagine(f_necriptata);

    unsigned int* R=generare_vector(img.latime,img.inaltime,R0);
    unsigned int* P=generare_permutare(img,R);

    imagine criptata;
    criptata.latime=img.latime;
    criptata.inaltime=img.inaltime;
    criptata.pixeli=(pixel*)malloc(criptata.inaltime*criptata.latime*sizeof(pixel));

    criptata=permuta_imagine(img, P);

    scriere_imagine(criptata, f_necriptata, f_criptata);

    free(img.pixeli);
    free(criptata.pixeli);
    free(R);
    free(P);
}

int main()
{
    /*
    char* necriptat=(char*)malloc(200);

    printf("cale fisier de criptat=");
    scanf("%s", necriptat);

    char* criptat=(char*)malloc(200);

    printf("cale fisier criptat=");
    scanf("%s", criptat);

    char* key=(char*)malloc(200);
    printf("cale fisier cheie secreta=");
    scanf("%s", key);

    criptare(necriptat, criptat, key);
*/

	criptare("peppers.bmp", "criptata.bmp", "secret_key.txt");

	/*
    free(necriptat);
    free(criptat);
    free(key);
*/

    return 0;
}
