#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>

size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    printf("%.*s", (int)realsize, (char *)contents);
    return realsize;
}

void perform_curl_request(const char *url) {
    CURL *curl;
    CURLcode res;

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

int main() {
    char user_choice;

    printf("Voulez-vous afficher toutes les questions (q) ou toutes les réponses (r) ? ");
    scanf("%c", &user_choice);

    if (user_choice == 'q') {
        perform_curl_request("http://localhost:8888/api-together/index.php?action=get_all_questions");
    } else if (user_choice == 'r') {
        perform_curl_request("http://localhost:8888/api-together/index.php?action=get_all_answers");
    } else {
        printf("Vous avez fait une erreur clavier.\n");
    }

    return 0;
}
