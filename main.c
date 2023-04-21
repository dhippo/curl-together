#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <string.h>

// Fonction de rappel pour écrire les données reçues
size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    printf("%.*s", (int)realsize, (char *)contents);
    return realsize;
}

// Fonction pour exécuter une requête cURL avec une URL et un nom d'activité (facultatif)
void perform_curl_request(const char *url, const char *nameActivity) {
    CURL *curl;
    CURLcode res;
    char url_with_keywords[1024];

    if (nameActivity != NULL) {
        snprintf(url_with_keywords, sizeof(url_with_keywords), "%s&nameActivity=%s", url, nameActivity);
        url = url_with_keywords;
    }

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
}

const char *find_activity_type(const char *word) {
    static const char* activity_types[][8] = {
            {"escapegame", "game", "escape", "escapegame", "roomescape", "escepe", "escepegem", "escepe"},
            {"evcaritatif", "association", "associatif", "benevolat", "ong", "caritatif", "humanitaire", "solidarite"},
            {"rallye", "decouverteville", "ville", "exploration", "urbaine", "rallye", "guidé", "promenade"},
            {"karaoke", "karaoké", "chorégraphie", "chant", "karaoke", "théatre", "chanson", "théâtre"},
            {"realitevirtuelle", "vr", "réalité", "immersif", "simulation", "3d", "casque", "virtuelle"},
            {"creationartistique", "peinture", "sculpture", "dessin", "art", "illustration", "artistique", "photographie"},
            {"competitionsportive", "sport", "sportive", "tournoi", "championnat", "match", "competition", "compétition"},
            {"apero", "cocktail", "repas", "soiree", "amuse-gueule", "apero", "apéro", "convivial"},
            {"atelierculinaire", "cuisine", "repas", "gastronomie", "recette", "cours", "dégustation", "ingredient"},
            {"babyfoot", "pong", "pingpong", "babyfoot", "billard", "jeuxdetable", "flechettes", "ping"},
            {"jeuxdesociete", "monopoly", "cartes", "plateau", "stratégie", "societe", "société", "puzzle"},
            {"randonnee", "rando", "randonné", "randonnee", "nature", "marche", "paysage", "sentier"},
            {"voyage", "week-end", "vacances", "excursion", "voyage", "aventure", "destination", "exploration"},
    };

    size_t num_activity_types = sizeof(activity_types) / sizeof(activity_types[0]);

    for (size_t i = 0; i < num_activity_types; i++) {
        for (size_t j = 1; j < 8; j++) {
            if (strcmp(word, activity_types[i][j]) == 0) {
                return activity_types[i][0];
            }
        }
    }
    return NULL;
}

// Fonction pour trouver le type de question en fonction d'un mot
const char *find_question_type(const char *word) {
    static const char* question_types[][5] = {
            {"duration", "date", "duree", "quand", "duree"},
            {"price", "cout", "coute", "prix", "chère"},
            {"description", "description", "desc", "expliquer", "explique"},
            {"types", "type", "catégorie", "categorie", "genre"},
            {"why", "pourquoi", "raison", "avantage", "utilité"},
    };

    size_t num_question_types = sizeof(question_types) / sizeof(question_types[0]);

    for (size_t i = 0; i < num_question_types; i++) {
        for (size_t j = 1; j < 5; j++) {
            if (strcmp(word, question_types[i][j]) == 0) {
                return question_types[i][0];
            }
        }
    }
    return NULL;
}

// Fonction pour traiter la question utilisateur et exécuter les requêtes cURL pour chaque mot de la phrase
void question_treatment(const char *input) {
    // Créez un tampon pour stocker la chaîne d'entrée modifiable
    char input_buffer[256];

    // Copiez la chaîne d'entrée dans le tampon et assurez-vous qu'elle se termine par un caractère nul
    strncpy(input_buffer, input, sizeof(input_buffer) - 1);
    input_buffer[sizeof(input_buffer) - 1] = '\0';

    // Commencez à analyser les mots de la chaîne d'entrée en les séparant par des espaces
    char *word = strtok(input_buffer, " ");
    const char *activity_type = NULL;
    const char *question_type = NULL;

    // Variables temporaires pour stocker les valeurs de activity_type et question_type trouvées lors de l'analyse des mots
    const char *temp_activity_type = NULL;
    const char *temp_question_type = NULL;

    // Continuez à analyser les mots jusqu'à ce qu'il n'y en ait plus
    while (word != NULL) {
        // Cherchez un activity_type pour le mot actuel à l'aide de la fonction find_activity_type
        temp_activity_type = find_activity_type(word);

        // Si un activity_type est trouvé, stockez-le dans la variable activity_type
        if (temp_activity_type) {
            activity_type = temp_activity_type;
        }

        // Cherchez un question_type pour le mot actuel à l'aide de la fonction find_question_type
        temp_question_type = find_question_type(word);

        // Si un question_type est trouvé, stockez-le dans la variable question_type
        if (temp_question_type) {
            question_type = temp_question_type;
        }

        // Passez au mot suivant en analysant à nouveau la chaîne d'entrée
        word = strtok(NULL, " ");
    }

    // Si un activity_type et un question_type sont trouvés, exécutez une requête cURL avec les activity_type et question_type comme paramètres
    if (activity_type && question_type) {
        char url[1024];
        snprintf(url, sizeof(url), "http://localhost:8888/api-together/index.php?action=specificInfo&activity_type=%s&question_type=%s", activity_type, question_type);
        perform_curl_request(url, NULL);
    }
}


int main() {
    char user_choice;
    char question[256];


        printf("Veuillez poser une question: ");
        fgets(question, sizeof(question), stdin);
        question[strcspn(question, "\n")] = 0; // Supprimer le caractère de nouvelle ligne

        question_treatment(question);


    return 0;
}
