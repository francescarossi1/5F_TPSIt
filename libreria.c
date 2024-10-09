#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LIBRI_MAX 100
#define CATEGORIE_MAX 20
#define MAX_TITOLO 100
#define MAX_AUTORE 100
#define MAX_CATEGORIA 50

typedef struct
{
    char titolo[MAX_TITOLO];
    char autore[MAX_AUTORE];
    int anno;
    float prezzo;
} Libro;

typedef struct
{
    char nome[MAX_CATEGORIA];
    Libro libri[LIBRI_MAX];
    int numLibri;
} Categoria;

Categoria categorie[CATEGORIE_MAX];
int numCategorie = 0;

void aggiungiLibro(Categoria *categorie, int *numCategorie, char nomeCategoria[], Libro nuovoLibro)
{
    int i;
    // Cerca la categoria
    for (i = 0; i < *numCategorie; i++)
    {
        if (strcmp(categorie[i].nome, nomeCategoria) == 0)
        {
            break; // Categoria trovata
        }
    }

    // Se la categoria non è stata trovata, la aggiunge
    if (i == *numCategorie)
    {
        if (*numCategorie >= CATEGORIE_MAX)
        {
            printf("Limite massimo di categorie raggiunto.\n");
            return;
        }
        strcpy(categorie[*numCategorie].nome, nomeCategoria);
        categorie[*numCategorie].numLibri = 0;
        (*numCategorie)++;
    }

    // Aggiunge il libro alla categoria
    if (categorie[i].numLibri < LIBRI_MAX)
    {
        categorie[i].libri[categorie[i].numLibri] = nuovoLibro;
        categorie[i].numLibri++;
    }
    else
    {
        printf("Limite massimo di libri per la categoria '%s' raggiunto.\n", nomeCategoria);
    }
}

int leggiCSV(char *nomeFile, Libro libri[], Categoria categorie[], int *numCategorie)
{
    FILE *file = fopen(nomeFile, "r");
    char risposta[3];
    if (!file)
    {
        printf("Errore nell'apertura del file.\n");
        return 0;
    }

    int count = 0;
    char riga[256];
    char nomeCategoria[MAX_CATEGORIA];

    // Salta la prima riga del file
    if (fgets(riga, sizeof(riga), file) == NULL)
    {
        printf("Errore nella lettura dell'intestazione.\n");
        fclose(file);
        return 0;
    }

    // Legge i dati riga per riga
    while (fgets(riga, sizeof(riga), file) && count < LIBRI_MAX)
    {
        char *pezzo = strtok(riga, ","); // Divide la riga in pezzi delimitati da una virgola
        if (pezzo)
            strcpy(libri[count].autore, pezzo); // Se pezzo diverso da null copia il valore di prezzo nella struct

        pezzo = strtok(NULL, ",");
        if (pezzo)
            strcpy(libri[count].titolo, pezzo);

        pezzo = strtok(NULL, ",");
        if (pezzo)
            libri[count].anno = atoi(pezzo);

        pezzo = strtok(NULL, ",");
        if (pezzo)
            libri[count].prezzo = atof(pezzo);

        count++; // Incrementa il conteggio dei libri
    }

    fclose(file);

    printf("Vuoi inserire una categoria per tutti i libri importati? (si o no): ");
    scanf("%s", risposta);
    getchar(); // Consuma il carattere newline rimasto da scanf

    if (strcmp(risposta, "si") == 0)
    {
        printf("Inserisci la categoria per i libri: ");
        fgets(nomeCategoria, MAX_CATEGORIA, stdin);
        nomeCategoria[strcspn(nomeCategoria, "\n")] = '\0'; // Rimuove il carattere di nuova linea
    }
    else
    {
        strcpy(nomeCategoria, "senza categoria"); // Categoria predefinita se non inserita
    }

    // Aggiungi i libri letti alla categoria specificata
    for (int i = 0; i < count; i++)
    {
        aggiungiLibro(categorie, numCategorie, nomeCategoria, libri[i]);
    }

    return count; // Ritorna il numero di libri letti
}

void aggiungiCategoria(Categoria *categorie, int *numCategorie, char nomeCategoria[])
{
    if (*numCategorie >= CATEGORIE_MAX)
    {
        printf("Limite massimo di categorie raggiunto.\n");
        return;
    }
    strcpy(categorie[*numCategorie].nome, nomeCategoria);
    categorie[*numCategorie].numLibri = 0;
    (*numCategorie)++;
}

void cercaLibro(Categoria *categorie, int numCategorie, char titolo[])
{
    for (int i = 0; i < numCategorie; i++)
    {
        for (int j = 0; j < categorie[i].numLibri; j++)
        {
            if (strcmp(categorie[i].libri[j].titolo, titolo) == 0)
            {
                printf("Libro trovato:\n");
                printf("Titolo: %s, Autore: %s, Anno: %d, Prezzo: %.2f\n",
                       categorie[i].libri[j].titolo,
                       categorie[i].libri[j].autore,
                       categorie[i].libri[j].anno,
                       categorie[i].libri[j].prezzo);
                return;
            }
        }
    }
    printf("Libro non trovato!\n");
}

void stampaLibriDiCategoria(Categoria *categorie, int numCategorie, char nomeCategoria[])
{
    for (int i = 0; i < numCategorie; i++)
    {
        if (strcmp(categorie[i].nome, nomeCategoria) == 0)
        {
            printf("Libri nella categoria %s:\n", nomeCategoria);
            for (int j = 0; j < categorie[i].numLibri; j++)
            {
                printf("Titolo: %s, Autore: %s, Anno: %d, Prezzo: %.2f\n",
                       categorie[i].libri[j].titolo,
                       categorie[i].libri[j].autore,
                       categorie[i].libri[j].anno,
                       categorie[i].libri[j].prezzo);
            }
            return;
        }
    }
    printf("Categoria non trovata!\n");
}

void stampaTutteLeCategorie(Categoria *categorie, int numCategorie)
{
    for (int i = 0; i < numCategorie; i++)
    {
        printf("Categoria: %s\n", categorie[i].nome);
        for (int j = 0; j < categorie[i].numLibri; j++)
        {
            printf("Titolo: %s, Autore: %s, Anno: %d, Prezzo: %.2f\n",
                   categorie[i].libri[j].titolo,
                   categorie[i].libri[j].autore,
                   categorie[i].libri[j].anno,
                   categorie[i].libri[j].prezzo);
        }
    }
}

void stampaCategorieSuFile(Categoria *categorie, int numCategorie, const char *filename)
{
    FILE *file = fopen(filename, "w");
    if (!file)
    {
        printf("Impossibile aprire il file.\n");
        return;
    }
    for (int i = 0; i < numCategorie; i++)
    {
        fprintf(file, "Categoria: %s\n", categorie[i].nome);
        for (int j = 0; j < categorie[i].numLibri; j++)
        {
            fprintf(file, "%s,%s,%d,%.2f\n",
                    categorie[i].libri[j].titolo,
                    categorie[i].libri[j].autore,
                    categorie[i].libri[j].anno,
                    categorie[i].libri[j].prezzo);
        }
    }
    fclose(file);
    printf("Categorie stampate nel file '%s'\n", filename);
}

void Menu()
{
    printf("Menu:\n");
    printf("1. Aggiungi Categoria\n");
    printf("2. Aggiungi Libro\n");
    printf("3. Cerca Libro\n");
    printf("4. Importa libri dal file\n");
    printf("5. Stampa Libri di Categoria\n");
    printf("6. Stampa Tutte le Categorie\n");
    printf("7. Stampa Categorie su File CSV\n");
    printf("8. Esci\n");
}

int main()
{
    int scelta;
    char nomeCategoria[MAX_CATEGORIA];
    Libro libriImportati[LIBRI_MAX];
    int numLibriImportati;

    do
    {
        Menu();
        printf("Seleziona un'opzione: ");
        scanf("%d", &scelta);
        getchar(); // Consuma il carattere newline rimasto da scanf

        switch (scelta)
        {
        case 1:
            printf("Inserisci nome della categoria: ");
            fgets(nomeCategoria, MAX_CATEGORIA, stdin);
            nomeCategoria[strcspn(nomeCategoria, "\n")] = '\0';
            aggiungiCategoria(categorie, &numCategorie, nomeCategoria);
            break;
        case 2:
        {
            Libro nuovoLibro;
            char nomeCategoria[MAX_CATEGORIA];

            printf("Inserisci il titolo del libro: ");
            fgets(nuovoLibro.titolo, MAX_TITOLO, stdin);
            nuovoLibro.titolo[strcspn(nuovoLibro.titolo, "\n")] = '\0'; // rimuove il carattere di nuova linea

            printf("Inserisci l'autore del libro: ");
            fgets(nuovoLibro.autore, MAX_AUTORE, stdin);
            nuovoLibro.autore[strcspn(nuovoLibro.autore, "\n")] = '\0';

            printf("Inserisci l'anno del libro: ");
            scanf("%d", &nuovoLibro.anno);

            printf("Inserisci il prezzo del libro: ");
            scanf("%f", &nuovoLibro.prezzo);
            getchar();

            printf("Inserisci la categoria del libro: ");
            fgets(nomeCategoria, MAX_CATEGORIA, stdin);
            nomeCategoria[strcspn(nomeCategoria, "\n")] = '\0';

            // aggiunge il libro alla categoria
            aggiungiLibro(categorie, &numCategorie, nomeCategoria, nuovoLibro);

            // salva i libri nel file CSV
            stampaCategorieSuFile(categorie, numCategorie, "libreria_libri.csv");
        }
        break;
        case 3:
            printf("Inserisci il titolo del libro da cercare: ");
            fgets(nomeCategoria, MAX_TITOLO, stdin);
            nomeCategoria[strcspn(nomeCategoria, "\n")] = '\0';
            cercaLibro(categorie, numCategorie, nomeCategoria);
            break;
        case 4:
            numLibriImportati = leggiCSV("libreria_libri.csv", libriImportati, categorie, &numCategorie);
            printf("Importati %d libri dal file.\n", numLibriImportati);
            break;
        case 5:
            printf("Inserisci il nome della categoria: ");
            fgets(nomeCategoria, MAX_CATEGORIA, stdin);
            nomeCategoria[strcspn(nomeCategoria, "\n")] = '\0';
            stampaLibriDiCategoria(categorie, numCategorie, nomeCategoria);
            break;
        case 6:
            stampaTutteLeCategorie(categorie, numCategorie);
            break;
        case 7:
            stampaCategorieSuFile(categorie, numCategorie, "tuttiILibri.csv");
            break;
        case 8:
            printf("Uscita...\n");
            break;
        default:
            printf("Opzione non valida, riprova.\n");
            break;
        }
    } while (scelta != 8);

    return 0;
}
