#include <iostream>
#include <cassert>

typedef struct livre {
    char* nom;
    int ID;
    struct livre* gauche;
    struct livre* droite;
} livre;

typedef int (*ordre)(char*, char*);
typedef void (*free_fn)(char*);
typedef void (*iterate)(char*);

typedef struct {
    size_t taille;
    livre* racine;
    ordre compare;
    free_fn libere;
} bibliotheque;

bibliotheque* creerBibliotheque(size_t taille, ordre compare, free_fn libere) {
    assert(taille > 0);
    assert(compare);

    bibliotheque* biblio = new bibliotheque;
    biblio->taille = taille;
    biblio->compare = compare;
    biblio->libere = libere;
    biblio->racine = nullptr;

    return biblio;
}

static livre* CreerLivre(int taille, char* element) {
    livre* livret = new livre;
    livret->nom = new char[taille];
    for (int i = 0; i < taille; ++i) {
        livret->nom[i] = element[i];
        if (element[i] == '\0') break;
    }
    livret->gauche = nullptr;
    livret->droite = nullptr;
    return livret;
}

static void AjouterLivre_rec(ordre compare, livre* courant, livre* nouveau) {
    if (compare(courant->nom, nouveau->nom) >= 0) {
        if (!courant->gauche)
            courant->gauche = nouveau;
        else
            AjouterLivre_rec(compare, courant->gauche, nouveau);
    } else {
        if (!courant->droite)
            courant->droite = nouveau;
        else
            AjouterLivre_rec(compare, courant->droite, nouveau);
    }
}

void AjouterLivre(bibliotheque* biblio, char* element) {
    assert(biblio);
    assert(element);
    livre* livret = CreerLivre(biblio->taille, element);
    if (biblio->racine)
        AjouterLivre_rec(biblio->compare, biblio->racine, livret);
    else
        biblio->racine = livret;
}

static void SupprimerLivre(bibliotheque* biblio, livre* livret) {
    if (biblio->libere) {
        biblio->libere(livret->nom);
    }
    delete[] livret->nom;
    delete livret;
}

static void SupprimerBibliotheque_rec(bibliotheque* biblio, livre* courant) {
    if (courant->gauche)
        SupprimerBibliotheque_rec(biblio, courant->gauche);
    if (courant->droite)
        SupprimerBibliotheque_rec(biblio, courant->droite);
    SupprimerLivre(biblio, courant);
}

void SupprimerBibliotheque(bibliotheque* biblio) {
    assert(biblio);
    if (biblio->racine)
        SupprimerBibliotheque_rec(biblio, biblio->racine);
    delete biblio;
}

static void ParcourirBibliotheque_rec(livre* courant, iterate fonction) {
    if (!courant) return;
    ParcourirBibliotheque_rec(courant->gauche, fonction);
    fonction(courant->nom);
    ParcourirBibliotheque_rec(courant->droite, fonction);
}

void ParcourirBibliotheque(bibliotheque* biblio, iterate fonction) {
    assert(biblio);
    assert(fonction);
    if (biblio->racine)
        ParcourirBibliotheque_rec(biblio->racine, fonction);
}

static livre* RechercherLivre_rec(ordre compare, livre* courant, char* element) {
    if (!courant)
        return nullptr;
    if (compare(courant->nom, element) > 0)
        return RechercherLivre_rec(compare, courant->gauche, element);
    else if (compare(courant->nom, element) < 0)
        return RechercherLivre_rec(compare, courant->droite, element);
    else
        return courant;
}

livre* RechercherLivre(bibliotheque* biblio, char* element, ordre compare) {
    assert(biblio);
    if (biblio->racine)
        return RechercherLivre_rec(compare, biblio->racine, element);
    return nullptr;
}

static livre* ChercherMax(livre* courant) {
    if (!courant->droite)
        return courant;
    return ChercherMax(courant->droite);
}

static livre* RetirerLivre_rec(bibliotheque* biblio, livre* courant, char* element) {
    if (!courant) return nullptr;

    if (biblio->compare(courant->nom, element) > 0) {
        courant->gauche = RetirerLivre_rec(biblio, courant->gauche, element);
    } else if (biblio->compare(courant->nom, element) < 0) { 
        courant->droite = RetirerLivre_rec(biblio, courant->droite, element);
    } else {
        //n'a pas denfants
        if (!courant->gauche && !courant->droite) {
            SupprimerLivre(biblio, courant);
            return nullptr;
        }
        //un seul enfant
        else if (!courant->droite) {
            livre* temp = courant->gauche;
            SupprimerLivre(biblio, courant);
            return temp;
        }
        else if (!courant->gauche) {
            livre* temp = courant->droite;
            SupprimerLivre(biblio, courant);
            return temp;
        }
        //deux enfants
        else {
            livre* successeur = ChercherMax(courant->gauche);
            for (size_t i = 0; i < biblio->taille; ++i) {
                courant->nom[i] = successeur->nom[i];
                if (successeur->nom[i] == '\0') break;
            }
            courant->gauche = RetirerLivre_rec(biblio, courant->gauche, successeur->nom);
        }
    }
    return courant;
}

void RetirerLivre(bibliotheque* biblio, char* element) {
    assert(biblio);
    if (biblio->racine)
        biblio->racine = RetirerLivre_rec(biblio, biblio->racine, element);
}

int comp(char* a, char* b) {
    while (*a && *b && *a == *b) {
        a++; b++;
    }
    return *a - *b;
}

void AfficherLivre(char* nom) {
    std::cout << "- " << nom << std::endl;
}

int main() {
    int choix;
    char nom[100];
    bibliotheque* Mareshall;

    std::cout << "Création de la bibliothèque..." << std::endl;
    Mareshall = creerBibliotheque(100, comp, nullptr);
    std::cout << "Fait" << std::endl;

    do {
        std::cout << "\n=== Accueil bibliothèque ===\n";
        std::cout << "1. Ajouter un livre" << std::endl;
        std::cout << "2. Rechercher un livre" << std::endl;
        std::cout << "3. Afficher les livres disponibles" << std::endl;
        std::cout << "4. Supprimer un livre" << std::endl;
        std::cout << "0. Quitter" << std::endl;
        std::cout << "Votre choix : ";
        std::cin >> choix;

        switch (choix) {
            case 1:
                std::cout << "Entrez le nom du livre à ajouter: ";
                std::cin >> nom;
                AjouterLivre(Mareshall, nom);
                std::cout << "Livre ajouté avec succès" << std::endl;
                break;
            case 2: {
                std::cout << "Entrez le nom du livre à rechercher: ";
                std::cin >> nom;
                livre* book = RechercherLivre(Mareshall, nom, comp);
                if (!book)
                    std::cout << "Aucune correspondance" << std::endl;
                else
                    std::cout << "Ce livre se trouve à l'adresse : " << book << std::endl;
                break;
            }
            case 3:
                std::cout << "Livres disponibles dans la bibliothèque:\n";
                ParcourirBibliotheque(Mareshall, AfficherLivre);
                break;
            case 4:
                std::cout << "Entrez le nom du livre à supprimer: ";
                std::cin >> nom;
                if (!RechercherLivre(Mareshall, nom, comp)) {
                    std::cout << "Aucune correspondance" << std::endl;
                } else {
                    RetirerLivre(Mareshall, nom);
                    std::cout << "Livre supprimé avec succès" << std::endl;
                }
                break;
            case 0:
                std::cout << "Au revoir !" << std::endl;
                break;
            default:
                std::cout << "Choix invalide !!!" << std::endl;
        }
    } while (choix != 0);

    SupprimerBibliotheque(Mareshall);
    return 0;
}
