#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <jansson.h>

#define API_KEY "27ed1a4db55a06d193a4f3e6b5ade1cb"
#define CITY_NAME "Córdoba,Argentina"
#define API_URL "http://api.openweathermap.org/data/2.5/weather?q="

struct MemoryStruct {
  char *memory;
  size_t size;
};

// Callback function to handle curl response
static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) 
{
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;

  mem->memory = realloc(mem->memory, mem->size + realsize + 1);
  if (mem->memory == NULL) {
    fprintf(stderr, "Not enough memory (realloc returned NULL)\n");
    return 0;
  }

  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;

  return realsize;
}

// Function to get weather information
void getWeather() {
  CURL *curl;
  CURLcode res;

  struct MemoryStruct chunk;

  chunk.memory = malloc(1);
  chunk.size = 0;

  curl_global_init(CURL_GLOBAL_DEFAULT);

  curl = curl_easy_init();
  if (curl) {
    char url[256];
    sprintf(url, "%s%s&appid=%s", API_URL, CITY_NAME, API_KEY);

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

    res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
      fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    } else {
      // Parse JSON response
      json_t *root;
      json_error_t error;

      root = json_loads(chunk.memory, 0, &error);

      if (root) {
        // Extract and display relevant information
        const char *description = json_string_value(json_object_get(json_array_get(json_object_get(root, "weather"), 0), "description"));
        double temperature_kelvin = json_real_value(json_object_get(json_object_get(root, "main"), "temp"));
        double temperature_celsius = temperature_kelvin - 273.15;
        int humidity = json_integer_value(json_object_get(json_object_get(root, "main"), "humidity"));
        double wind_speed = json_real_value(json_object_get(json_object_get(root, "wind"), "speed"));

        printf("Weather in %s: %s\n", CITY_NAME, description);
        printf("Temperature: %.2f°C\n", temperature_celsius);
        printf("Humidity: %d%%\n", humidity);
        printf("Wind Speed: %.2f m/s\n", wind_speed);

        // Clean up JSON object
        json_decref(root);
      } else {
        fprintf(stderr, "Error parsing JSON: %s\n", error.text);
      }
    }

    curl_easy_cleanup(curl);
    free(chunk.memory);
  }

  curl_global_cleanup();
}

int main(void) {
  getWeather();
  return 0;
}

/*
Chat gtp explains code:
Certainly! In the provided C code snippet, a structure (struct) named `MemoryStruct` is defined. This structure contains two members:

1. `char *memory`: This is a pointer to a character (char) array or buffer. It is used to store dynamic memory allocated during the program's execution. The pointer allows the program to access and manipulate a block of memory.

2. `size_t size`: This member is of the type `size_t`, which is an unsigned integer type defined in the standard library (usually 32 or 64 bits, depending on the system). It represents the size or length of the memory block pointed to by `char *memory`. The `size` member is used to keep track of the size of the dynamically allocated memory.

This structure is often used in the context of functions that perform dynamic memory allocation, such as when retrieving data from a network or reading from a file. The `MemoryStruct` structure helps organize and manage the allocated memory and its size.

This code defines a callback function named `WriteMemoryCallback`, which is typically used as a callback for the `CURLOPT_WRITEFUNCTION` option in a libcurl request. This function is designed to handle the received data during an HTTP request and store it in a dynamically allocated memory block.

Let's break down the code:

1. `void *contents`: This parameter represents the received data (the content of the HTTP response).

2. `size_t size`: The size of each data element.

3. `size_t nmemb`: The number of data elements.

4. `void *userp`: A user-defined pointer that can be passed to the callback function. In this case, it is expected to be a pointer to a `struct MemoryStruct`.

Inside the function:

- `size_t realsize = size * nmemb;`: Calculate the total size of the received data.

- `struct MemoryStruct *mem = (struct MemoryStruct *)userp;`: Cast the user-defined pointer back to a pointer of type `struct MemoryStruct`. This allows the function to access and manipulate the `MemoryStruct` structure.

- `mem->memory = realloc(mem->memory, mem->size + realsize + 1);`: Reallocate memory for the data. It uses `realloc` to resize the memory block pointed to by `mem->memory`. The new size is calculated by adding the current size (`mem->size`) and the size of the received data (`realsize`). The `+ 1` is for accommodating the null terminator for string termination.

- Check if the reallocation was successful: `if (mem->memory == NULL) {  handle error  }`. If the reallocation fails (returns `NULL`), an error message is printed to the standard error stream.

This function is commonly used when using libcurl to perform HTTP requests and dynamically storing the received data. The dynamic reallocation is crucial to ensure that enough memory is available as the data is received in chunks during the request.

The `getWeather` function is designed to retrieve weather information from an API using libcurl and process the JSON response to display relevant weather details. Let's break down the functionality of the function:

1. **Initialization:**
   - Initialize a `CURL` handle (`curl`) for making HTTP requests.
   - Initialize a structure named `chunk` of type `MemoryStruct` to store the received data.

   ```c
   CURL *curl;
   CURLcode res;
   struct MemoryStruct chunk;
   chunk.memory = malloc(1);
   chunk.size = 0;
   ```

2. **Curl Global Initialization:**
   - Initialize the libcurl library globally.

   ```c
   curl_global_init(CURL_GLOBAL_DEFAULT);
   ```

3. **Curl Easy Initialization and Set Options:**
   - Initialize the `CURL` handle.
   - Construct the URL for the weather API request using predefined constants (`API_URL`, `CITY_NAME`, `API_KEY`).
   - Set the URL, write callback function (`WriteMemoryCallback`), and the user data (pointer to `chunk`) for libcurl.

   ```c
   curl = curl_easy_init();
   if (curl) {
      // ... (setting URL, write callback, and user data)
   }
   ```

4. **Perform HTTP Request:**
   - Execute the HTTP request using `curl_easy_perform`.

   ```c
   res = curl_easy_perform(curl);
   ```

5. **Handle Response:**
   - Check if the request was successful or not.
   - If successful, parse the JSON response using the Jansson library.
   - Extract relevant weather information such as description, temperature, humidity, and wind speed.

   ```c
   if (res != CURLE_OK) {
      // Handle curl request failure
   } else {
      // Parse JSON response
      // Extract and display relevant information
   }
   ```

6. **Clean Up:**
   - Cleanup libcurl resources and free the dynamically allocated memory for the response data.

   ```c
   curl_easy_cleanup(curl);
   free(chunk.memory);
   ```

7. **Global Cleanup:**
   - Cleanup libcurl global resources.

   ```c
   curl_global_cleanup();
   ```

In summary, this function performs an HTTP request to a weather API, processes the JSON response to extract weather details, and prints the information to the console. The use of dynamic memory allocation and proper cleanup ensures efficient resource management.
*/
