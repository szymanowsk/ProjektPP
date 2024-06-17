#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <windows.h>

#define WORD_SIZE 50
int globalneId = 1;

// struktura do przechowywania daty
typedef struct Data
{
    int rok;
    int miesiac;
    int dzien;
} Data;

typedef struct przedmiot
{
    int id;
    char nazwa[WORD_SIZE];
    char kategoria[WORD_SIZE];
    float kwotaZastawu;

    time_t data;
    float cena;
    bool czySprzedany; // true - jest, false - sprzedany
    struct przedmiot *next;
} lista;

void dodaj(lista **p, int id, char *nazwa, char *kategoria, float kwota, Data data, float cena)
{
    lista* tmp = *p;
    lista* nowy = (lista*)malloc(sizeof(lista));
    nowy->next = NULL;
    nowy->id = id;
    strcpy(nowy->nazwa, nazwa);
    strcpy(nowy->kategoria, kategoria);
    nowy->kwotaZastawu = kwota;

    struct tm tm_date = {0};
    tm_date.tm_year = data.rok - 1900;
    tm_date.tm_mon = data.miesiac - 1;
    tm_date.tm_mday = data.dzien;
    nowy->data = mktime(&tm_date);

    nowy->cena = cena;
    nowy->czySprzedany = true;

    if ((*p) == NULL)
    {
        (*p) = nowy;
    }
    else
    {
        while (tmp->next != NULL)
        {
            tmp = tmp->next;
        }
        tmp->next = nowy;
    }
    globalneId++;
}

void wypiszPrzedmiot(lista *p)
{
    struct tm *data_info;
    data_info = localtime(&(p->data));
    printf("id: %d, %s, kat: %s, zastaw: %.2f, data: %d-%02d-%02d, cena: %.2f, %d\n", p->id, p->nazwa, p->kategoria,
           p->kwotaZastawu, data_info->tm_year + 1900, data_info->tm_mon + 1, data_info->tm_mday, p->cena, p->czySprzedany);
}

void wypiszListe(lista *p)
{
    lista *tmp = p;
    while (tmp != NULL)
    {
        wypiszPrzedmiot(tmp);
        tmp = tmp->next;
    }
}

void oznaczJakoSprzedany(lista *p, int id)
{
    while (p != NULL)
    {
        if (p->id == id)
        {
            p->czySprzedany = false;
            return;
        }
        p = p->next;
    }
}

void edytuj(lista *p, int id)
{
    lista *tmp = p;
    while(tmp != NULL)
    {
        if (tmp->id == id)
        {
            int wybor;
            int dzien, miesiac, rok;

            do
            {
                printf("Co chcesz edytowac?\n");
                printf("1. Nazwa\n");
                printf("2. Kategoria\n");
                printf("3. Kwota zastawu\n");
                printf("4. Data zastawienia\n");
                printf("5. Cena\n");
                printf("6. Status\n");
                printf("7. Zakoncz edycje\n");
                scanf("%d", &wybor);

                switch (wybor)
                {
                    case 1:
                        printf("Nowa nazwa: ");
                        getchar();
                        fgets(tmp->nazwa, WORD_SIZE, stdin);
                        tmp->nazwa[strcspn(tmp->nazwa, "\n")] = 0; //usuniecie znaku "\n"
                        break;
                    case 2:
                        printf("Nowa kategoria: ");
                        getchar();
                        fgets(tmp->kategoria, WORD_SIZE, stdin);
                        tmp->kategoria[strcspn(tmp->kategoria, "\n")] = 0;
                        break;
                    case 3:
                        printf("Nowa kwota zastawu: ");
                        getchar();
                        scanf("%f", &tmp->kwotaZastawu);
                        break;
                    case 4:
                        printf("Nowa data (RRRR-MM-DD): ");
                        getchar();
                        scanf("%d-%d-%d", &rok, &miesiac, &dzien);
                        struct tm date = {0};
                        date.tm_year = rok - 1900;
                        date.tm_mon = miesiac - 1;
                        date.tm_mday = dzien;
                        tmp->data = mktime(&date);
                        break;
                    case 5:
                        printf("Nowa cena: ");
                        getchar();
                        scanf("%f", &tmp->cena);
                        break;
                    case 6:
                        printf("Nowy status (0 - sprzedany, 1 - w bazie): ");
                        int stan;
                        getchar();
                        scanf("%d", &stan);
                        tmp->czySprzedany = stan;
                        break;
                    case 7:
                        break;
                    default:
                        printf("Niepoprawny wybor.\n");
                        break;
                }
            }
            while (wybor != 7);

            return;
        }
        tmp = tmp->next;
    }
    printf("Nie znaleziono przedmiotu o podanym ID.\n");
}

void usun(lista **p, int id)
{
    lista *tmp = *p;
    lista *prev = NULL;

    if (tmp == NULL)
        return;

    if (tmp != NULL && tmp->id == id && tmp->czySprzedany)
    {
        //prev = tmp;
        //tmp = tmp->next;
        (*p) = tmp->next;
        free(prev);
        //(*p) = tmp;
        return;
    }

    while (tmp != NULL && (tmp->id != id || !tmp->czySprzedany))
    {
        prev = tmp;
        tmp = tmp->next;
    }

    if (tmp == NULL)
    {
        printf("Nie mozna usunac elementu o podanym ID\n");
        return;
    }

    prev->next = tmp->next;

    free(tmp);
}

void wczytajDane(char *plik, lista **p)
{
    FILE *fp = NULL;
    fp = fopen(plik, "r");
    if(fp==NULL)
    {
        printf("Nie mozna otworzyc pliku do odczytu.");
        exit(-1);
    }

    char nazwa[WORD_SIZE];
    char kategoria[WORD_SIZE];
    float kwotaZastawu;
    int rok, miesiac, dzien;
    float cena;
    //bool flaga;

    while (fscanf(fp, "%s %s %f %d-%d-%d %f", nazwa, kategoria, &kwotaZastawu, &rok, &miesiac, &dzien, &cena) != EOF)
    {
        Data data = {rok, miesiac, dzien};

        char fnazwa[WORD_SIZE] = {};
        char* token = strtok(nazwa, "+");

        while (token != NULL)
        {
            strcat(fnazwa, token);
            token = strtok(NULL, "+");
            if(token == NULL)
                break;
            strcat(fnazwa, " ");
        }
        dodaj(p, globalneId, fnazwa, kategoria, kwotaZastawu, data, cena);
    }

    fclose(fp);
}

void zapiszDane(char *plik, lista *p)
{
    FILE *fp = fopen(plik, "w");
    if(fp == NULL)
    {
        printf("Nie mozna otworzyc pliku do zapisu.");
        exit(-1);
    }

    while (p != NULL)
    {
        struct tm *data_info;
        data_info = localtime(&(p->data));

        char nazwa[WORD_SIZE] = {};
        char* token = strtok(p->nazwa, " ");

        while (token != NULL)
        {
            strcat(nazwa, token);
            token = strtok(NULL, " ");
            if(token == NULL)
                break;
            strcat(nazwa, "+");
        }

        fprintf(fp, "%d %s %s %.2f %d-%02d-%02d %.2f %d\n", p->id, nazwa, p->kategoria, p->kwotaZastawu,
                data_info->tm_year + 1900, data_info->tm_mon + 1, data_info->tm_mday, p->cena,
                p->czySprzedany);
        p = p->next;
    }

    fclose(fp);
}

void wyszukajNazwa(lista *p, char *nazwa)
{
    lista *tmp = p;
    while (tmp != NULL)
    {
        if (strstr(tmp->nazwa, nazwa) != NULL)
        {
            wypiszPrzedmiot(tmp);
        }
        tmp = tmp->next;
    }
}

void wyszukajCena(lista *p, float min, float max)
{
    lista *tmp = p;
    while (tmp != NULL)
    {
        if (tmp->cena >= min && tmp->cena <= max)
        {
            wypiszPrzedmiot(tmp);
        }
        tmp = tmp->next;
    }
}

void wyszukajData(lista *p, Data data)
{
    lista *tmp = p;
    while (tmp != NULL)
    {
        struct tm *data_info;
        data_info = localtime(&(tmp->data));
        if (data_info->tm_year + 1900 == data.rok &&
            data_info->tm_mon + 1 == data.miesiac &&
            data_info->tm_mday == data.dzien)
        {
            wypiszPrzedmiot(tmp);
        }
        tmp = tmp->next;
    }
}

void wyswietlKategoria(lista *p, char *kategoria)
{
    lista *tmp = p;
    while (tmp != NULL)
    {
        if (strcmp(tmp->kategoria, kategoria) == 0)
        {
            wypiszPrzedmiot(tmp);
        }
        tmp = tmp->next;
    }
}

void sortujNazwa(lista **p)
{
    lista *tmp = *p;
    int swapped;
    lista *ptr = NULL;

    if (tmp == NULL)
        return;

    do
    {
        swapped = 0;
        tmp = *p;

        while (tmp->next != ptr)
        {
            if (strcmp(tmp->nazwa, tmp->next->nazwa) > 0)
            {
                char temp[WORD_SIZE];
                strcpy(temp, tmp->nazwa);
                strcpy(tmp->nazwa, tmp->next->nazwa);
                strcpy(tmp->next->nazwa, temp);

                swapped = 1;
            }
            tmp = tmp->next;
        }
        ptr = tmp;
    }
    while (swapped);
}

void sortujData(lista **p)
{
    lista *tmp = *p;
    int swapped;
    lista *ptr = NULL;

    if (tmp == NULL)
        return;

    do
    {
        swapped = 0;
        tmp = *p;

        while (tmp->next != ptr)
        {
            if (difftime(tmp->data, tmp->next->data) > 0)
            {
                lista *temp = tmp->next;
                tmp->next = temp->next;
                temp->next = tmp;

                if (tmp == *p)
                    *p = temp;
                else
                {
                    lista *prev = *p;
                    while (prev->next != tmp)
                        prev = prev->next;
                    prev->next = temp;
                }
                swapped = 1;
            }
            else
            {
                tmp = tmp->next;
            }
        }
        ptr = tmp;
    }
    while (swapped);
}

void sortujStatus(lista **p)
{
    lista *tmp = *p;
    int swapped;
    lista *ptr = NULL;

    if (tmp == NULL)
        return;

    do
    {
        swapped = 0;
        tmp = *p;

        while (tmp->next != ptr)
        {
            if (tmp->czySprzedany && !(tmp->next->czySprzedany))
            {
                lista *temp = tmp->next;
                tmp->next = temp->next;
                temp->next = tmp;

                if (tmp == *p)
                    *p = temp;
                else
                {
                    lista *prev = *p;
                    while (prev->next != tmp)
                        prev = prev->next;
                    prev->next = temp;
                }
                swapped = 1;
            }
            else
            {
                tmp = tmp->next;
            }
        }
        ptr = tmp;
    }
    while (swapped);
}

void menu()
{
    printf("\nMENU:\n");
    printf("1. Wypisz liste przedmiotow\n");
    printf("2. Dodaj przedmiot\n");
    printf("3. Edytuj przedmiot\n");
    printf("4. Usun przedmiot\n");
    printf("5. Wyszukaj przedmiot po nazwie\n");
    printf("6. Wyszukaj przedmiot po przedziale cenowym\n");
    printf("7. Wyszukaj przedmiot po dacie\n");
    printf("8. Wyswietl przedmioty z danej kategorii\n");
    printf("9. Wyswietl przedmioty posortowane wedlug nazwy\n");
    printf("10. Wyswietl przedmioty posortowane wedlug daty\n");
    printf("11. Wyswietl przedmioty posortowane wedlug statusu\n");
    printf("12. Wyjdz z programu\n");
}

int main()
{
    lista *p = NULL;

    printf(" __        ______   .___  ___. .______        ___      .______    _______  \n");
    printf("|  |      /  __  \\  |   \\/   | |   _  \\      /   \\     |   _  \\  |       \\ \n");
    printf("|  |     |  |  |  | |  \\  /  | |  |_)  |    /  ^  \\    |  |_)  | |  .--.  |\n");
    printf("|  |     |  |  |  | |  |\\/|  | |   _  <    /  /_\\  \\   |      /  |  |  |  |\n");
    printf("|  `----.|  `--'  | |  |  |  | |  |_)  |  /  _____  \\  |  |\\  \\-.|  '--'  |\n");
    printf("|_______| \\______/  |__|  |__| |______/  /__/     \\__\\ | _| `.__||_______/\n");

    wczytajDane("dane.txt", &p);

    int wybor;
    do
    {
        menu();
        printf("Wybierz opcje: ");
        scanf("%d", &wybor);
        switch (wybor)
        {
            case 1:
                wypiszListe(p);
                break;
            case 2:
                printf("Podaj nazwe: ");
                char nazwaU[WORD_SIZE];
                getchar();
                fgets(nazwaU, WORD_SIZE, stdin);
                nazwaU[strcspn(nazwaU, "\n")] = 0; // usuwa znak nowej linii
                printf("Podaj kategorie: ");
                char kategoriaU[WORD_SIZE];
                fgets(kategoriaU, WORD_SIZE, stdin);
                kategoriaU[strcspn(kategoriaU, "\n")] = 0;
                printf("Podaj kwote zastawu: ");
                float kwotaU;
                scanf("%f", &kwotaU);
                printf("Podaj date zastawienia (RRRR-MM-DD): ");
                Data dataU;
                scanf("%d-%d-%d", &dataU.rok, &dataU.miesiac, &dataU.dzien);
                printf("Podaj cene: ");
                float cenaU;
                scanf("%f", &cenaU);

                dodaj(&p, globalneId, nazwaU, kategoriaU, kwotaU, dataU, cenaU);
                break;
            case 3:
                printf("Podaj id:");
                int idU;
                getchar();
                scanf("%d", &idU);

                edytuj(p, idU);
                break;
            case 4:
                printf("Podaj id:");
                getchar();
                scanf("%d", &idU);

                usun(&p, idU);
                break;
            case 5:
                printf("Podaj nazwe do wyszukania: ");
                char szukanaNazwa[WORD_SIZE];
                getchar();
                fgets(szukanaNazwa, WORD_SIZE, stdin);
                szukanaNazwa[strcspn(szukanaNazwa, "\n")] = 0;

                wyszukajNazwa(p, szukanaNazwa);
                break;
            case 6:
                printf("Podaj minimalna cene: ");
                float min, max;
                getchar();
                scanf("%f", &min);
                printf("Podaj maksymalna cene: ");
                getchar();
                scanf("%f", &max);

                wyszukajCena(p, min, max);
                break;
            case 7:
                printf("Podaj date do wyszukania (RRRR-MM-DD): ");
                Data data;
                getchar();
                scanf("%d-%d-%d", &data.rok, &data.miesiac, &data.dzien);

                wyszukajData(p, data);
                break;
            case 8:
                printf("Podaj kategorie do wyszukania: ");
                char szukanaKategoria[WORD_SIZE];
                getchar();
                fgets(szukanaKategoria, WORD_SIZE, stdin);
                szukanaKategoria[strcspn(szukanaKategoria, "\n")] = 0;

                wyswietlKategoria(p, szukanaKategoria);
                break;
            case 9:
                sortujNazwa(&p);
                wypiszListe(p);
                break;
            case 10:
                sortujData(&p);
                wypiszListe(p);
                break;
            case 11:
                sortujStatus(&p);
                wypiszListe(p);
                break;
            case 12:
                zapiszDane("dane_out.txt", p);
                break;
            default:
                printf("Niepoprawny wybor.\n");
                break;
        }
    }
    while (wybor != 12);

    return 0;
}
