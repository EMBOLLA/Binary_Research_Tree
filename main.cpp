#include <iostream>
#include <cassert>

typedef struct livre{
    char* nom;
    int ID;
    struct livre* gauche;
    struct livre* droite;
}livre;

typedef int (*ordre)(char*, char*);//va permettre la comparaison entre les noeuds
typedef void (*free)(char *); //va permettre la désallocation des noeuds
typedef void (*iterate)(char*);//va nous servir à parcourir la bibliothèque 

typedef struct{
    size_t taille;
    livre* racine;
    ordre compare;
    free libere;
} bibliotheque;


bibliotheque* creerBibliotheque(size_t taille, ordre compare, free libere){
    assert(taille > 0);
    assert(compare);

    bibliotheque* biblio = (bibliotheque *)malloc(sizeof(bibliotheque));
    if (biblio)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    biblio->taille = taille;
    biblio->compare = compare;
    biblio->libere = libere;
    biblio ->racine = NULL;

    return biblio;
    
}

static livre* CreerLivre(int taille, char* element){
    livre* livret = (livre *)malloc(sizeof(livre));
    if(!livret){
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    livret->nom = (char*)malloc(sizeof(taille));
    if (!(livret->nom))
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    memcpy(livret->nom, element, taille);
    livret->gauche = NULL;
    livret->droite = NULL;

    return livret;
}

static void AjouterLivre_rec(ordre compare, livre* courant,livre* nouveau ){
    if (compare(courant->nom, nouveau->nom) >=0 ){
        if (!courant->gauche)
        {
            courant->gauche = nouveau;
        }else{
             AjouterLivre_rec(compare, courant->gauche, nouveau);

        }
        
    }else{
      if (!courant->droite)
        {
            courant->droite = nouveau;
        }else{
             AjouterLivre_rec(compare, courant->droite, nouveau);

        }  
    }
    
    
}

void AjouterLivre(bibliotheque* biblio,char* element ){
    assert(biblio);
    assert(element);
    //création du livre(noeud)
    livre* livret = CreerLivre(biblio->taille,element);
    if (biblio->racine)//si la bibliothèque a déjà une racine
    {
        AjouterLivre_rec(biblio->compare, biblio->racine, livret);
    }else
    {
        biblio->racine = livret;
    }
    
    
}

static void SupprimerLivre(bibliotheque* biblio, livre* livret){
    if(biblio->libere){
        biblio->libere(livret->nom);
    }
    free(livret->nom);
    free(livret);
}

static void SupprimerBibliotheque_rec(bibliotheque* biblio, livre* courant){
    if(courant->gauche){
        SupprimerBibliotheque_rec(biblio, courant->gauche);
    }
    if(courant->droite){
        SupprimerBibliotheque_rec(biblio, courant->droite);
    }
    SupprimerLivre(biblio, courant);

}

void SupprimerBibliotheque(bibliotheque* biblio){
    assert(biblio);
    if(biblio->racine){
        SupprimerBibliotheque_rec(biblio, biblio->racine);
    }
    free(biblio);
}

static void ParcourirBibliotheque_rec(livre* courant, iterate fonction){
    if (!courant)
    {
        return;
    }
    ParcourirBibliotheque_rec(courant->gauche, fonction);
    fonction(courant->nom);
    ParcourirBibliotheque_rec(courant->droite, fonction);

}


void ParcourirBibliotheque(bibliotheque* biblio, iterate fonction){
    assert(biblio);
    assert(fonction);
    if (biblio->racine)
    ParcourirBibliotheque_rec(biblio->racine, fonction);
    
}

static livre* RechercherLivre_rec(ordre compare, livre* courant,char* element){
    if (!courant)
    {
        return NULL;
    }
    if (compare(courant->nom, element) > 0)
    {
        return RechercherLivre_rec(compare, courant->gauche, element);
    }else if (compare(courant->nom, element) < 0)
    {
                return RechercherLivre_rec(compare, courant->droite, element);
    }else{
        return courant;
    }
    
    
    
}

livre* RechercherLivre(bibliotheque* biblio, char* element, ordre compare){
    assert(biblio);
    if (biblio->racine)
    {
        return RechercherLivre_rec(compare, biblio->racine, element);
    }
    return NULL;
    
}


int comp(char* a, char* b){
    return *a - *b;
}

int main(int argc, char* argv[]){
    int choix;
    bibliotheque* Mareshall;
    int taille = 5;
    std::cout<<"Création de la bibliothèque..."<<std::endl;
    Mareshall = creerBibliotheque(sizeof(int), comp, NULL);
    std::cout<<"Fait"<<std::endl;
    do{
        std::cout << "=== Acceuil bibliothèque ===\n";
        std::cout << "1. Ajouter un livre"<<std::endl;
        std::cout << "2. Rechercher un livre"<<std::endl;
        std::cout << "3. Afficher les livres disponibles"<<std::endl;
        std::cout << "0. Quitter "<<std::endl;
        std::cout << "Votre choix : ";
        std::cin >> choix;

        switch (choix)
        {
        case 1:
            char nom[20];
            std::cout<<"Entrez le nom du livre à ajouter"<<std::endl;
            std::cin>>nom;
            AjouterLivre(Mareshall, nom);
            std::cout<<"Livre ajouté avec succès";
            break;
        case 2:
            char nom[20];
            livre* book;
            std::cout<<"Entrez le nom du livre à rechercher"<<std::endl;
            std::cin>>nom;
            book = RechercherLivre(Mareshall, nom, comp);
            if (book == NULL)
            {
                std::cout<<"Aucune correspondance"<<std::endl;
            }else{
                std::cout<<"Ce livre se trouve à l'adresse"<<book<<std::endl;
            }
            break;
        case 3:

            break;
        default:
        std::cout<<"Choix invalide !!!"<<std::endl;
            break;
        }
    }while(choix != 0);
}
