#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <jansson.h>

#define DEEPL_API_KEY "05b4bb3d-a214-4510-a553-1780e8ec975a:fx"
#define DEEPL_API_URL "https://api-free.deepl.com/v2/translate"

struct MemoryStruct {
    char *memory;
    size_t size;
};

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    mem->memory = realloc(mem->memory, mem->size + realsize + 1);
    if (mem->memory == NULL) {
        fprintf(stderr, "No se pudo asignar memoria.\n");
        return 0;
    }

    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

char *extractTranslation(const char *jsonResponse) {
    // Parsear el JSON
    json_t *root;
    json_error_t error;
    root = json_loads(jsonResponse, 0, &error);

    if (!root) {
        fprintf(stderr, "Error al parsear JSON: %s\n", error.text);
        return NULL;
    }

    // Extraer la traducción
    json_t *translations = json_object_get(root, "translations");
    if (!json_array_size(translations)) {
        fprintf(stderr, "No se encontraron traducciones en la respuesta JSON.\n");
        json_decref(root);
        return NULL;
    }

    json_t *translation = json_array_get(translations, 0);
    json_t *translatedText = json_object_get(translation, "text");

    const char *translationStr = json_string_value(translatedText);
    char *result = strdup(translationStr);

    // Liberar memoria del JSON
    json_decref(root);

    return result;
}

void translateText(const char *textToTranslate) {
    CURL *curl;
    CURLcode res;

    struct MemoryStruct chunk;

    chunk.memory = malloc(1);
    chunk.size = 0;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        // Codificar espacios en blanco en el texto a traducir
        char *encodedText = curl_easy_escape(curl, textToTranslate, 0);

        char url[512];
        snprintf(url, sizeof(url), "%s?auth_key=%s&text=%s&source_lang=ES&target_lang=EN", DEEPL_API_URL, DEEPL_API_KEY, encodedText);

        curl_free(encodedText);

        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            fprintf(stderr, "Error en la solicitud HTTP: %s\n", curl_easy_strerror(res));
        } else {
            char *translation = extractTranslation(chunk.memory);
            if (translation) {
                printf("Traducción: %s\n", translation);
                free(translation);
            }
        }

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        free(chunk.memory);
    }

    curl_global_cleanup();
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s \"Texto a traducir\"\n", argv[0]);
        return 1;
    }

    const char *textToTranslate = argv[1];
    translateText(textToTranslate);

    return 0;
}

